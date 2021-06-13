#include "peer.h"
#include "block.h"
#include "piece.h"
#include "jtorrent.h"
#include "torrent.h"
#include "torrentinfo.h"
#include "torrentmessage.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QDebug>

const int BLOCK_REQUEST_SIZE = 16384;
const int REPLY_TIMEOUT_MSEC = 10000;
const int HANDSHAKE_TIMEOUT_MSEC = 20000;
const int BLOCKS_TO_REQUEST = 5;
const int MAX_MESSAGE_LENGTH = 65536;
const int RECONNECT_INTERVAL_MSEC = 30000;
const int SEND_MESSAGES_INTERVAL = 1000;

Peer::Peer(ConnectionInitiator connectionInitiator, QTcpSocket *socket)
    : _torrent(nullptr)
    , _bitfield(nullptr)
    , _state(Created)
    , _connectionInitiator(connectionInitiator)
    , _socket(socket)
    , _isPaused(false)
{
	connectAll();
}

Peer::~Peer()
{
    delete[] _bitfield;
    delete _socket;
}


void Peer::startConnection()
{
    if (_connectionInitiator == ConnectionInitiator::Peer) {
		qDebug() << "Peer::startConnection(): Called, but connection was initiated by the Peer";
		return;
	}
    if (_socket->isOpen()) {
        _socket->close();
	}

    _piecesDownloaded = 0;
    for (int i = 0; i < _torrent->torrentInfo()->bitfieldSize() * 8; i++) {
        _bitfield[i] = false;
	}
    _protocol.clear();
    _reserved.clear();
    _infoHash.clear();
    _peerId.clear();

    _state = Connecting;
    _amChoking = true;
    _amInterested = false;
    _peerChoking = true;
    _peerInterested = false;

    _receivedDataBuffer.clear();
    _replyTimeoutTimer.stop();
    _handshakeTimeoutTimer.stop();
    _reconnectTimer.stop();

    _sendMessagesTimer.stop();

    _hasTimedOut = false;
    _blocksQueue.clear();

	qDebug() << "Connecting to" << addressPort();
    _socket->connectToHost(_address, _port);
}

void Peer::start()
{
    if(_connectionInitiator == ConnectionInitiator::Peer) {
		return;
	}

    _isPaused = false;
    if (_state == ConnectionEstablished) {
		sendMessages();
    } else if (_socket->state() == QAbstractSocket::UnconnectedState) {
		startConnection();
	}
}

void Peer::pause()
{
    _isPaused = true;
	sendMessages();
}

void Peer::sendHandshake()
{
	QByteArray dataToWrite;
	dataToWrite.push_back(char(19));
	dataToWrite.push_back("BitTorrent protocol");
	for (int i = 0; i < 8; i++) {
		dataToWrite.push_back(char(0));
	}
    dataToWrite.push_back(_torrent->torrentInfo()->infoHash());
    dataToWrite.push_back(JTorrent::instance()->peerId());
    _socket->write(dataToWrite);
}

void Peer::sendChoke()
{
    if (_state != ConnectionEstablished) {
		return;
	}
    _amChoking = true;
    TorrentMessage::choke(_socket);
}

void Peer::sendUnchoke()
{
    if (_state != ConnectionEstablished) {
		return;
	}
    _amChoking = false;
    TorrentMessage::unchoke(_socket);
}

void Peer::sendInterested()
{
    if (_state != ConnectionEstablished) {
		return;
	}
    _amInterested = true;
    TorrentMessage::interested(_socket);
}

void Peer::sendNotInterested()
{
    if (_state != ConnectionEstablished) {
		return;
	}
    _amInterested = false;
    TorrentMessage::notInterested(_socket);
}

void Peer::sendHave(int index)
{
    if (_state != ConnectionEstablished) {
		return;
	}
    TorrentMessage::have(_socket, index);
}

void Peer::sendBitfield()
{
    if (_state != ConnectionEstablished) {
		return;
	}
    TorrentMessage::bitfield(_socket, _torrent->bitfield());
}

void Peer::sendRequest(Block* block)
{
    if (_state != ConnectionEstablished) {
		return;
	}
	block->addAssignee(this);

	int index = block->piece()->pieceNumber();
	int begin = block->begin();
	int length = block->size();
	qDebug() << "Request" << index << begin << length << "from" << addressPort();
    TorrentMessage::request(_socket, index, begin, length);

    _replyTimeoutTimer.start();

    _blocksQueue.push_back(block);
}

void Peer::sendPiece(int index, int begin, const QByteArray &blockData)
{
    if (_state != ConnectionEstablished) {
		return;
	}
	qDebug() << "Sending piece" << index << begin << blockData.size() << "to" << addressPort();
    TorrentMessage::piece(_socket, index, begin, blockData);
    _torrent->onBlockUploaded(blockData.size());
	emit uploadedData(blockData.size());
}

void Peer::sendCancel(Block* block)
{
    if (_state != ConnectionEstablished) {
		return;
	}
	int	index = block->piece()->pieceNumber();
	int begin = block->begin();
	int length = block->size();
    TorrentMessage::cancel(_socket, index, begin, length);
}

bool Peer::requestBlock()
{
    Block *block = _torrent->requestBlock(this, BLOCK_REQUEST_SIZE);
	if (block == nullptr) {
		return false;
	}
	sendRequest(block);
	return true;
}

void Peer::disconnect()
{
	qDebug() << "Disconnecting from" << addressPort();
	if (isConnected()) {
        _socket->close();
	} else {
		finished();
	}
}

void Peer::fatalError()
{
	qDebug() << "Fatal error with" << addressPort() << "; Dropping connection";
    _state = Error;
    _socket->close();
}

Peer *Peer::createClient(QTcpSocket *socket)
{
	Peer *peer = new Peer(ConnectionInitiator::Peer, socket);
	peer->initClient();
	return peer;
}

Peer *Peer::createServer(Torrent *torrent, QHostAddress address, int port)
{
	Peer *peer = new Peer(ConnectionInitiator::Client, new QTcpSocket);
	peer->initServer(torrent, address, port);
	return peer;
}

void Peer::sendMessages()
{
    _sendMessagesTimer.stop();

    if (_state != ConnectionEstablished) {
		return;
	}

    if (_torrent->isDownloaded() && isDownloaded()) {
		disconnect();
		return;
	}

    _sendMessagesTimer.start(SEND_MESSAGES_INTERVAL);

    if (_isPaused) {

        if (_amInterested) {
			sendNotInterested();
		}
        if (!_amChoking) {
			sendChoke();
		}
        for (Block *block : _blocksQueue) {
			sendCancel(block);
		}
		releaseAllBlocks();

	} else {

        if (!_amInterested) {
			if (isInteresting()) {
				sendInterested();
			}
		}
        if (_peerInterested && _amChoking) {
			sendUnchoke();
		}
        if (!_peerChoking && _amInterested) {
            while (_blocksQueue.size() < BLOCKS_TO_REQUEST) {
				if (!requestBlock()) {
					break;
				}
			}
		}

	}
}


bool Peer::readHandshakeReply(bool *ok)
{
	*ok = true;

    if (_receivedDataBuffer.isEmpty()) {
		return false;
	}
	int i = 0;
    int protocolLength = _receivedDataBuffer[i++];
    if (_receivedDataBuffer.size() < 49 + protocolLength) {
		return false;
	}

	for (int j = 0; j < protocolLength; j++) {
        _protocol.push_back(_receivedDataBuffer[i++]);
	}
	for (int j = 0; j < 8; j++) {
        _reserved.push_back(_receivedDataBuffer[i++]);
	}
	for (int j = 0; j < 20; j++) {
        _infoHash.push_back(_receivedDataBuffer[i++]);
	}
	for (int j = 0; j < 20; j++) {
        _peerId.push_back(_receivedDataBuffer[i++]);
	}
    _receivedDataBuffer.remove(0, 49 + protocolLength);

    if (_connectionInitiator == ConnectionInitiator::Client) {
        if (_infoHash != _torrent->torrentInfo()->infoHash()) {
			qDebug() << "Info hash does not match expected one from peer" << addressPort();
			*ok = false;
			return false;
		}
	} else {
        _torrent = nullptr;
        for (auto torrent : JTorrent::instance()->torrents()) {
            if (torrent->torrentInfo()->infoHash() == _infoHash) {
                _torrent = torrent;
				break;
			}
		}
        if (_torrent == nullptr) {
            qDebug() << "No torrents matching info hash" << _infoHash.toHex() << "for" << addressPort();
			*ok = false;
			return false;
		}
	}
	return true;
}

bool Peer::readPeerMessage(bool *ok)
{
	*ok = true;

    if (_receivedDataBuffer.size() < 4) {
		return false;
	}

	int i = 0;

	int length = 0;
	for (int j = 0; j < 4; j++) {
		length *= 256;
        length += (unsigned char)_receivedDataBuffer[i++];
	}

	if (length > MAX_MESSAGE_LENGTH || length < 0) {
		*ok = false;
		return false;
	}

    if (length == 0) {
		qDebug() << addressPort() << ": keep-alive";
        _receivedDataBuffer.remove(0, i);
		return true;
	}

    if (_receivedDataBuffer.size() < 4 + length) {
		return false;
	}

    int messageId = _receivedDataBuffer[i++];
	switch (messageId) {
	case TorrentMessage::Choke: {
		qDebug() << addressPort() << ": choke";
        _peerChoking = true;
		releaseAllBlocks();
        _replyTimeoutTimer.stop();
        _hasTimedOut = false;
		break;
	}
	case TorrentMessage::Unchoke: {
		qDebug() << addressPort() << ": unchoke";
        _peerChoking = false;
		break;
	}
	case TorrentMessage::Interested: {
		qDebug() << addressPort() << ": interested";
        _peerInterested = true;
		break;
	}
	case TorrentMessage::NotInterested: {
		qDebug() << addressPort() << ": not interested";
        _peerInterested = false;
		break;
	}
	case TorrentMessage::Have: {
		int pieceNumber = 0;
		for (int j = 0; j < 4; j++) {
			pieceNumber *= 256;
            pieceNumber += (unsigned char)_receivedDataBuffer[i++];
		}
        if (!_bitfield[pieceNumber]) {
            _bitfield[pieceNumber] = true;
            _piecesDownloaded++;
		}
		break;
	}
	case TorrentMessage::Bitfield: {
		int bitfieldSize = length - 1;
        if (bitfieldSize != _torrent->torrentInfo()->bitfieldSize()) {
			qDebug() << "Error: Peer" << addressPort() << "sent bitfield of wrong size:" << bitfieldSize*8
                     << "expected" << _torrent->torrentInfo()->bitfieldSize();
			*ok = false;
			return false;
		} else {
			for (int j = 0; j < bitfieldSize; j++) {
                unsigned char byte = _receivedDataBuffer[i++];
				unsigned char pos = 0b10000000;
				for (int q = 0; q < 8; q++) {
                    _bitfield[j * 8 + q] = ((byte & pos) != 0);
					pos = pos >> 1;
				}
			}
            _piecesDownloaded = 0;
			for (int j = 0; j < bitfieldSize * 8; j++) {
                if (_bitfield[j]) {
                    _piecesDownloaded++;
				}
			}
		}
		break;
	}
	case TorrentMessage::Request: {
		unsigned int index = 0;
		unsigned int begin = 0;
		unsigned int blockLength = 0;

		for (int j = 0; j < 4; j++) {
			index *= 256;
            index += (unsigned char)_receivedDataBuffer[i++];
		}
		for (int j = 0; j < 4; j++) {
			begin *= 256;
            begin += (unsigned char)_receivedDataBuffer[i++];
		}
		for (int j = 0; j < 4; j++) {
			blockLength *= 256;
            blockLength += (unsigned char)_receivedDataBuffer[i++];
		}

        QList<Piece *> &pieces = _torrent->pieces();

		if (index >= (unsigned)pieces.size() || blockLength > MAX_MESSAGE_LENGTH) {
			qDebug() << "Invalid request (" << index << begin << blockLength << ")"
					 << "from" << addressPort();
			disconnect();
			*ok = false;
			return false;
		}
		Piece *piece = pieces[index];

		if (begin + blockLength > (unsigned)piece->size() || begin > (unsigned)piece->size()) {
			qDebug() << "Invalid request (" << index << begin << blockLength << ")"
					 << "from" << addressPort();
			disconnect();
			*ok = false;
			return false;
		}

		QByteArray blockData;
		if (!piece->getBlockData(begin, blockLength, blockData)) {
			qDebug() << "Failed to get block (" << index << begin << blockLength << ")"
					 << "for" << addressPort();
			disconnect();
			*ok = false;
			return false;
		}

		sendPiece(index, begin, blockData);

		break;
	}
	case TorrentMessage::Piece: {
		int index = 0;
		int begin = 0;
		int blockLength = length - 9;
		for (int j = 0; j < 4; j++) {
			index *= 256;
            index += (unsigned char)_receivedDataBuffer[i++];
		}
		for (int j = 0; j < 4; j++) {
			begin *= 256;
            begin += (unsigned char)_receivedDataBuffer[i++];
		}
		Block *block = nullptr;
		int blockIndex = 0;
        for (auto b : _blocksQueue) {
			if (b->piece()->pieceNumber() == index
				&& b->begin() == begin
				&& b->size() == blockLength) {
				block = b;
				break;
			}
			blockIndex++;
		}

		if (block == nullptr) {
            QList<Piece *> pieces = _torrent->pieces();
			if (index >= 0 && index < pieces.size()) {
				block = pieces[index]->getBlock(begin, blockLength);
			}
		}

		if (block == nullptr) {
			qDebug() << "Error: received unrequested block from peer" << addressPort()
					 << ". Block(" << index << begin << blockLength << ")";
		} else {
            _hasTimedOut = false;
            const char *blockData = _receivedDataBuffer.data() + i;
			if (!block->isDownloaded()) {
				block->setData(this, blockData);
				releaseBlock(block);
				emit downloadedData(blockLength);
			}
            if (_blocksQueue.isEmpty()) {
                _replyTimeoutTimer.stop();
			} else {
                _replyTimeoutTimer.start();
			}
		}
		break;
	}
	case TorrentMessage::Cancel: {
		break;
	}
	case TorrentMessage::Port: {
		break;
	}
	default:
		qDebug() << "Error: Received unknown message with id =" << messageId
				 << " and length =" << length << "from" << addressPort();
		*ok = false;
		return false;
	}
    _receivedDataBuffer.remove(0, 4 + length);
	return true;
}

void Peer::connectAll()
{
    connect(_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(finished()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));

    connect(&_replyTimeoutTimer, SIGNAL(timeout()), this, SLOT(replyTimeout()));
    connect(&_handshakeTimeoutTimer, SIGNAL(timeout()), this, SLOT(handshakeTimeout()));
    connect(&_reconnectTimer, SIGNAL(timeout()), this, SLOT(reconnect()));
    connect(&_sendMessagesTimer, SIGNAL(timeout()), this, SLOT(sendMessages()));

    _replyTimeoutTimer.setInterval(REPLY_TIMEOUT_MSEC);
    _handshakeTimeoutTimer.setInterval(HANDSHAKE_TIMEOUT_MSEC);
    _reconnectTimer.setInterval(RECONNECT_INTERVAL_MSEC);
}

void Peer::initBitfield()
{
    int bitfieldSize = _torrent->torrentInfo()->bitfieldSize();
    _bitfield = new bool[bitfieldSize * 8];
	for (int i = 0; i < bitfieldSize * 8; i++) {
        _bitfield[i] = false;
	}
}

void Peer::initClient()
{
    _torrent = nullptr;
    _address = _socket->peerAddress();
    _port = _socket->peerPort();
    _piecesDownloaded = 0;
    _bitfield = nullptr;
    _state = Handshaking;

    _protocol.clear();
    _reserved.clear();
    _infoHash.clear();
    _peerId.clear();

    _amChoking = true;
    _amInterested = false;
    _peerChoking = true;
    _peerInterested = false;

    _receivedDataBuffer.clear();
    _replyTimeoutTimer.stop();
    _handshakeTimeoutTimer.stop();
    _reconnectTimer.stop();

    _sendMessagesTimer.stop();

    _hasTimedOut = false;
    _blocksQueue.clear();
}

void Peer::initServer(Torrent *torrent, QHostAddress address, int port)
{
    _torrent = torrent;
    _address = address;
    _port = port;
    _piecesDownloaded = 0;
	initBitfield();
    _state = Created;
}

void Peer::releaseBlock(Block *block)
{
	block->removeAssignee(this);
    _blocksQueue.removeAll(block);
	if (!block->hasAssignees() && !block->isDownloaded()) {
		block->piece()->deleteBlock(block);
	}
}

void Peer::releaseAllBlocks()
{
    QList<Block *> blocks = _blocksQueue;
	for (Block *block : blocks) {
		block->removeAssignee(this);
        _blocksQueue.removeAll(block);
		if (!block->hasAssignees() && !block->isDownloaded()) {
			block->piece()->deleteBlock(block);
		}
	}
}

void Peer::connected()
{
	qDebug() << "Connected to" << addressPort();

    _state = Handshaking;
	sendHandshake();
    _handshakeTimeoutTimer.start();
}

void Peer::readyRead()
{
    _receivedDataBuffer.push_back(_socket->readAll());

    switch (_state) {
	case Handshaking:
		bool ok;
		if (readHandshakeReply(&ok)) {
            if (_connectionInitiator == ConnectionInitiator::Peer) {
                if(_torrent->state() != Torrent::Started) {
					disconnect();
					break;
				}
				initBitfield();
				sendHandshake();
                _torrent->addPeer(this);
			}
		} else {
			if (!ok) {
				fatalError();
			}
			break;
		}
        _handshakeTimeoutTimer.stop();
		qDebug() << "Handshaking completed with peer" << addressPort();
        _state = ConnectionEstablished;
        _sendMessagesTimer.start(SEND_MESSAGES_INTERVAL);
		sendBitfield();
	case ConnectionEstablished: {
		int messagesReceived = 0;
		while (readPeerMessage(&ok)) {
			messagesReceived++;
		}
		if (!ok) {
			fatalError();
			break;
		}

		if (messagesReceived) {
			sendMessages();
		}
		break;
	}
	default:
        _receivedDataBuffer.clear();
		break;
	}
}

void Peer::finished()
{
    _handshakeTimeoutTimer.stop();
    _replyTimeoutTimer.stop();
    _sendMessagesTimer.stop();
	releaseAllBlocks();
    if (_state != Error) {
        _state = Disconnected;
	}

    if (_connectionInitiator == ConnectionInitiator::Client) {
        if (!isDownloaded() || !_torrent->isDownloaded()) {
            _reconnectTimer.start();
		}
	}
    qDebug() << "Connection to" << addressPort() << "closed" << _socket->errorString();
}

void Peer::error(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Socket error" << addressPort() << ":"
             << _socket->errorString() << "(" << socketError << ")";
	disconnect();
}

void Peer::replyTimeout()
{
	qDebug() << "Peer" << addressPort() << "took too long to reply";
    _hasTimedOut = true;
    _replyTimeoutTimer.stop();
}

void Peer::handshakeTimeout()
{
	qDebug() << "Peer" << addressPort() << "took too long to handshake";
    _handshakeTimeoutTimer.stop();
}

void Peer::reconnect()
{
	qDebug() << "Reconnecting to" << addressPort();
    _reconnectTimer.stop();
    if (_torrent->isStarted()) {
		startConnection();
	}
}

Torrent *Peer::torrent()
{
    return _torrent;
}

QHostAddress Peer::address()
{
    return _address;
}

int Peer::port()
{
    return _port;
}

int Peer::piecesDownloaded()
{
    return _piecesDownloaded;
}

bool *Peer::bitfield()
{
    return _bitfield;
}

QByteArray &Peer::protocol()
{
    return _protocol;
}

QByteArray &Peer::reserved()
{
    return _reserved;
}

QByteArray &Peer::infoHash()
{
    return _infoHash;
}

QByteArray &Peer::peerId()
{
    return _peerId;
}

Peer::State Peer::state()
{
    return _state;
}

Peer::ConnectionInitiator Peer::connectionInitiator()
{
    return _connectionInitiator;
}

bool Peer::amChoking()
{
    return _amChoking;
}

bool Peer::amInterested()
{
    return _amInterested;
}

bool Peer::peerChoking()
{
    return _peerChoking;
}

bool Peer::peerInterested()
{
    return _peerInterested;
}

QTcpSocket *Peer::socket()
{
    return _socket;
}

bool Peer::hasTimedOut()
{
    return _hasTimedOut;
}

QList<Block *> &Peer::blocksQueue()
{
    return _blocksQueue;
}

bool Peer::isPaused() const
{
    return _isPaused;
}

QString Peer::addressPort()
{
    return _address.toString() + ":" + QString::number(_port);
}

bool Peer::isDownloaded()
{
    return _piecesDownloaded == _torrent->torrentInfo()->numberOfPieces();
}

bool Peer::hasPiece(Piece *piece)
{
    return _bitfield[piece->pieceNumber()];
}

bool Peer::isConnected()
{
    return _socket->state() == QAbstractSocket::ConnectedState;
}

bool Peer::isInteresting()
{
    if (_torrent->isDownloaded()) {
		return false;
	}
    for (Piece* piece : _torrent->pieces()) {
		if (!piece->isDownloaded() && hasPiece(piece)) {
			return true;
		}
	}
	return false;
}
