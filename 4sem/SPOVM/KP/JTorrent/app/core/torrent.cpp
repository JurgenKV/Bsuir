#include "torrent.h"
#include "peer.h"
#include "torrentinfo.h"
#include "piece.h"
#include "block.h"
#include "torrentmessage.h"
#include "jtorrent.h"
#include "filecontroller.h"
#include "trafficmonitor.h"
#include "ui/mainwindow.h"
#include <QDir>
#include <QFile>
#include <QUrlQuery>

Torrent::Torrent()
    : _state(New)
    , _torrentInfo(nullptr)
    , _trackerClient(nullptr)
    , _fileController(nullptr)
    , _trafficMonitor(new TrafficMonitor(this))
    , _bytesDownloadedOnStartup(0)
    , _bytesUploadedOnStartup(0)
    , _totalBytesDownloaded(0)
    , _totalBytesUploaded(0)
    , _bytesAvailable(0)
    , _downloadedPieces(0)
    , _isDownloaded(false)
    , _isPaused(true)
    , _startAfterChecking(false)
{
}

Torrent::~Torrent()
{
    for (auto peer : _peers) {
		delete peer;
	}

    for (Piece *piece : _pieces) {
		delete piece;
	}

    if (_torrentInfo) {
        delete _torrentInfo;
	}

    if (_trackerClient) {
        delete _trackerClient;
	}

    for (QFile *file : _files) {
		delete file;
	}

    if (_fileController) {
        delete _fileController;
	}
}

bool Torrent::createNew(TorrentInfo *torrentInfo, const QString &downloadLocation)
{
	clearError();
    _state = Loading;

    _torrentInfo = torrentInfo;
    _downloadLocation = downloadLocation;

    for (int i = 0; i < _torrentInfo->numberOfPieces() - 1; i++) {
        _pieces.push_back(new Piece(this, i, _torrentInfo->pieceLength()));
	}
    int lastPieceLength = _torrentInfo->length() % _torrentInfo->pieceLength();
	if (lastPieceLength == 0) {
       lastPieceLength = _torrentInfo->pieceLength();
	}
    _pieces.push_back(new Piece(this, _torrentInfo->numberOfPieces() - 1, lastPieceLength));

    _trackerClient = new TrackerClient(this);

    _fileController = new FileController(this);
    connect(this, &Torrent::checkingStarted, _fileController, &FileController::checkTorrent);
    connect(_fileController, &FileController::torrentChecked, this, &Torrent::onChecked);

    _state = Stopped;

	loadFileDescriptors();

	return true;
}

bool Torrent::createFromResumeInfo(TorrentInfo *torrentInfo, ResumeInfo *resumeInfo)
{
	clearError();
    _state = Loading;

    _torrentInfo = torrentInfo;

    for (int i = 0; i < _torrentInfo->numberOfPieces() - 1; i++) {
        _pieces.push_back(new Piece(this, i, _torrentInfo->pieceLength()));
	}
    int lastPieceLength = _torrentInfo->length() % _torrentInfo->pieceLength();
	if (lastPieceLength == 0) {
        lastPieceLength = _torrentInfo->pieceLength();
	}
    _pieces.push_back(new Piece(this, _torrentInfo->numberOfPieces() - 1, lastPieceLength));

    _trackerClient = new TrackerClient(this);

    _fileController = new FileController(this);
    connect(this, &Torrent::checkingStarted, _fileController, &FileController::checkTorrent);
    connect(_fileController, &FileController::torrentChecked, this, &Torrent::onChecked);

    if (_pieces.size() != resumeInfo->aquiredPieces().size()) {
		setError("The number of pieces in the TorrentInfo does not match the one in the ResumeInfo");
		return false;
	}

    for (Piece *piece : _pieces) {
		if (resumeInfo->aquiredPieces().at(piece->pieceNumber())) {
			setPieceAvailable(piece, true);
		}
	}

    _downloadLocation = resumeInfo->downloadLocation();

    _totalBytesDownloaded = resumeInfo->totalBytesDownloaded();
    _totalBytesUploaded = resumeInfo->totalBytesUploaded();

    _state = Stopped;

	loadFileDescriptors();

	if (resumeInfo->paused()) {
		pause();
	} else {
		start();
	}

	return true;
}

void Torrent::loadFileDescriptors()
{
    const QList<FileInfo> &fileInfos = _torrentInfo->fileInfos();
	for (int i = 0; i < fileInfos.size(); i++) {
		FileInfo info = fileInfos[i];
        QString path = _downloadLocation;
		if (path[path.size() - 1] != '/') {
			path.append('/');
		}
		for (int j = 0; j < info.path.size() - 1; j++) {
			path.append(info.path[j]);
			path.append('/');
		}
		QDir dir;
		dir.mkpath(path);
		path += info.path.last();
        _files.append(new QFile(path));
	}
}

ResumeInfo Torrent::getResumeInfo() const
{
    ResumeInfo resumeInfo(_torrentInfo);
    resumeInfo.setDownloadLocation(_downloadLocation);
    resumeInfo.setTotalBytesDownloaded(_totalBytesDownloaded);
    resumeInfo.setTotalBytesUploaded(_totalBytesUploaded);
    resumeInfo.setPaused(_isPaused);
	resumeInfo.setAquiredPieces(bitfield());
	return resumeInfo;
}

void Torrent::start()
{
    if (_state == Checking) {
        _startAfterChecking = true;
		return;
	}
    if (!_trackerClient->hasAnnouncedStarted()) {
        _trackerClient->announce(TrackerClient::Started);
    } else if (_trackerClient->numberOfAnnounces() == 0) {
        _trackerClient->announce(TrackerClient::None);
	}

    for (Peer *peer :  _peers) {
		peer->start();
	}
    _isPaused = false;
    _state = Started;
}

void Torrent::pause()
{
    for (Peer *peer : _peers) {
		peer->pause();
	}
    _isPaused = true;
}

void Torrent::stop()
{
    if (_state != Started) {
		return;
	}
    if (_trackerClient->hasAnnouncedStarted()) {
        _trackerClient->announce(TrackerClient::Stopped);
	}
    for (Peer *peer : _peers) {
		peer->disconnect();
	}
    _state = Stopped;
}

void Torrent::check()
{
    if (_state == Started) {
		stop();
        _startAfterChecking = true;
    } else if (_state != Stopped) {
		return;
	}
    _state = Checking;
	emit checkingStarted();
}

Peer *Torrent::connectToPeer(QHostAddress address, int port)
{
    for (auto p : _peers) {
		if (p->port() == port && p->address() == address) {
			return nullptr;
		}
	}
	Peer *peer = Peer::createServer(this, address, port);
    _trafficMonitor->addPeer(peer);
    _peers.push_back(peer);

	if(isStarted()) {
		peer->startConnection();

        if (_isPaused) {
			peer->pause();
		}
	}

	qDebug() << "Added peer" << peer->addressPort();
	return peer;
}

void Torrent::addPeer(Peer *peer)
{
    for (Peer *&p : _peers) {
		if (p->port() == peer->port() && p->address() == peer->address()) {
			p->deleteLater();
			p = peer;
			return;
		}
	}
    _peers.push_back(peer);
    _trafficMonitor->addPeer(peer);
	qDebug() << "Added peer" << peer->addressPort();
}

Block *Torrent::requestBlock(Peer *peer, int size)
{
	Block *returnBlock = nullptr;
    for (auto piece : _pieces) {
		if (!piece->isDownloaded() && peer->hasPiece(piece)) {
			returnBlock = piece->requestBlock(size);
			if (returnBlock != nullptr) {
				return returnBlock;
			}
		}
	}

    for (auto peer : _peers) {
		if (peer->hasTimedOut()) {
			for (auto block : peer->blocksQueue()) {
				if (peer->hasPiece(block->piece())) {
					return block;
				}
			}
		}
	}

	return nullptr;
}

bool Torrent::savePiece(Piece *piece)
{
    int pieceLength = _torrentInfo->pieceLength();
	int thisPieceLength = piece->size();
    const QList<FileInfo> &fileInfos = _torrentInfo->fileInfos();

	qint64 dataBegin = piece->pieceNumber();
	dataBegin *= pieceLength;

	int firstFileNumber = 0;
	qint64 startingPos = 0;
	qint64 pos = 0;
	for (int i = 0; i < fileInfos.size(); i++) {
		pos += fileInfos[i].length;
		firstFileNumber = i;
		if (pos > dataBegin) {
			pos -= fileInfos[i].length;
			startingPos = dataBegin - pos;
			break;
		}
	}

	char *dataPtr = piece->data();

	int bytesToWrite = thisPieceLength;
	for (int i = firstFileNumber;; i++) {
		FileInfo info = fileInfos[i];
		if (info.path.size() > 1) {
            QString path = _downloadLocation;
			if (path[path.size()-1] == '/') {
				path.remove(path.size()-1, 1);
			}
			for (int j = 0; j < info.path.size() - 1; j++) {
				path += '/';
				path += info.path[j];
			}
			QDir dir;
			dir.mkpath(path);
		}

        QFile *file = _files[i];
		if (!file->exists() || file->size() != info.length) {
			file->resize(info.length);
		}
        if (!file->open(QIODevice::ReadWrite)) {
			qDebug() << "Failed to open file" << i << file->fileName() << ":" << file->errorString();
			return false;
		}
		qint64 pos = 0;
		if (i == firstFileNumber) {
			pos = startingPos;
		}
		if (!file->seek(pos)) {
			qDebug() << "Failed to seek in file" << i << file->fileName() << ":" << file->errorString();
			return false;
		}
		int toWrite = bytesToWrite;
		qint64 bytesToEnd = fileInfos[i].length - pos;
		if (toWrite > bytesToEnd) {
			toWrite = bytesToEnd;
		}
		for (int written = 0;;) {
			dataPtr += written;
			toWrite -= written;
			bytesToWrite -= written;
			if (toWrite == 0) {
				break;
			}
			written = file->write(dataPtr, toWrite);
			if (written == -1) {
				qDebug() << "Failed to write to file" << i << file->fileName() << ":" << file->errorString();
				file->close();
				return false;
			}
		}
		file->close();
		if (bytesToWrite == 0) {
			break;
		}
	}
	return true;
}

void Torrent::setPieceAvailable(Piece *piece, bool available)
{
	if (piece->isDownloaded() == available) {
		return;
	}

	if (available) {
        _downloadedPieces++;
        _bytesAvailable += piece->size();

        if (_state == Started) {
            _totalBytesDownloaded += piece->size();

            for (auto peer : _peers) {
				peer->sendHave(piece->pieceNumber());
			}
		}
        if (_downloadedPieces == _torrentInfo->numberOfPieces()) {
			onFullyDownloaded();
		}
	} else {
        _downloadedPieces--;
        _bytesAvailable -= piece->size();
        _isDownloaded = false;
	}

	piece->setDownloaded(available);
}

void Torrent::onSuccessfullyAnnounced(TrackerClient::Event event)
{
	if (event == TrackerClient::Started) {
        _bytesDownloadedOnStartup = _totalBytesDownloaded;
        _bytesUploadedOnStartup = _totalBytesUploaded;
	}
}


QList<Peer *> &Torrent::peers()
{
    return _peers;
}

QList<Piece *> &Torrent::pieces()
{
    return _pieces;
}

TorrentInfo *Torrent::torrentInfo()
{
    return _torrentInfo;
}

TrackerClient *Torrent::trackerClient()
{
    return _trackerClient;
}

TrafficMonitor *Torrent::trafficMonitor()
{
    return _trafficMonitor;
}

QList<QFile *> &Torrent::files()
{
    return _files;
}


qint64 Torrent::bytesDownloaded() const
{
    if (!_trackerClient->hasAnnouncedStarted()) {
		return 0;
	}
    return _totalBytesDownloaded - _bytesDownloadedOnStartup;
}

qint64 Torrent::bytesUploaded() const
{
    if (!_trackerClient->hasAnnouncedStarted()) {
		return 0;
	}
    return _totalBytesUploaded - _bytesUploadedOnStartup;
}

qint64 Torrent::totalBytesDownloaded() const
{
    return _totalBytesDownloaded;
}

qint64 Torrent::totalBytesUploaded() const
{
    return _totalBytesUploaded;
}

qint64 Torrent::bytesAvailable() const
{
    return _bytesAvailable;
}

qint64 Torrent::bytesLeft() const
{
    return _torrentInfo->length() - bytesAvailable();
}


int Torrent::downloadedPieces()
{
    return _downloadedPieces;
}

bool Torrent::isDownloaded()
{
    return _isDownloaded;
}

bool Torrent::isPaused() const
{
    return _isPaused;
}

bool Torrent::isStarted() const
{
    return !_isPaused && _state == Started;
}

int Torrent::connectedPeersCount() const
{
	int count = 0;
    for (Peer *peer : _peers) {
		if (peer->isConnected()) {
			count++;
		}
	}
	return count;
}

int Torrent::allPeersCount() const
{
    return _peers.size();
}

Torrent::State Torrent::state() const
{
    return _state;
}

QString Torrent::stateString() const
{
    switch (_state) {
	case New:
		return "Created";
	case Loading:
		return "Loading";
	case Checking:
		return "Checking";
	case Stopped:
		return "Stopped";
	default:
        if (_isPaused) {
			return "Paused";
        } else if (_isDownloaded) {
			return "Completed";
		} else {
			return "Downloading";
		}
	}
}

const QString &Torrent::downloadLocation() const
{
    return _downloadLocation;
}


float Torrent::percentDownloaded()
{
    double percent = _bytesAvailable;
    percent /= _torrentInfo->length();
	percent *= 100.0f;
	return percent;
}

QVector<bool> Torrent::bitfield() const
{
	QVector<bool> bf;
    for (auto p : _pieces) {
		bf.push_back(p->isDownloaded());
	}
	return bf;
}


QString Torrent::errorString() const
{
    return _errorString;
}


void Torrent::onPieceDownloaded(Piece *piece)
{
    _downloadedPieces++;
    _bytesAvailable += piece->size();
    _totalBytesDownloaded += piece->size();

	qDebug() << "Downloaded pieces"
             << _downloadedPieces << "/" << _torrentInfo->numberOfPieces()
			 << "=" << percentDownloaded() << "%";

    for (auto peer : _peers) {
		peer->sendHave(piece->pieceNumber());
	}

    if (_downloadedPieces == _torrentInfo->numberOfPieces()) {
		onFullyDownloaded();
	}
}

void Torrent::onBlockUploaded(int bytes)
{
    _totalBytesUploaded += bytes;
}

void Torrent::onFullyDownloaded()
{
    if (_state == Started) {
        JTorrent::instance()->mainWindow()->torrentFullyDownloaded(this);
        _trackerClient->announce(TrackerClient::Completed);
	}
    _isDownloaded = true;
    for (auto peer : _peers) {
		if (peer->isDownloaded() || peer->isConnected()) {
			peer->disconnect();
		}
	}
	emit fullyDownloaded();
}

void Torrent::clearError()
{
    _errorString.clear();
}

void Torrent::setError(const QString &errorString)
{
    _errorString = errorString;
}


void Torrent::onChecked()
{
    _state = Stopped;
    if (_startAfterChecking) {
		start();
	}
	emit checked();
}
