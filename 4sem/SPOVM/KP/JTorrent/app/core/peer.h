#ifndef PEER_H
#define PEER_H

#include <QByteArray>
#include <QHostAddress>
#include <QTimer>
#include <QObject>
#include <QAbstractSocket>

class Torrent;
class Piece;
class Block;
class QTcpSocket;


class Peer : public QObject
{
	Q_OBJECT

public:
	enum State {
        Created,
        Connecting,
        Handshaking,
        ConnectionEstablished,
        Disconnected,
        Error
	};

	enum class ConnectionInitiator
	{
        Client,
        Peer
	};

	Torrent *torrent();

	QHostAddress address();
	int port();
	int piecesDownloaded();
	bool *bitfield();
	QByteArray &protocol();
	QByteArray &reserved();
	QByteArray &infoHash();
	QByteArray &peerId();

	State state();
	ConnectionInitiator connectionInitiator();
	bool amChoking();
	bool amInterested();
	bool peerChoking();
	bool peerInterested();

	QTcpSocket *socket();
	bool hasTimedOut();
	QList<Block *> &blocksQueue();
	bool isPaused() const;

	QString addressPort();
	bool isDownloaded();
	bool hasPiece(Piece *piece);
	bool isConnected();
	bool isInteresting();

private:
    Torrent *_torrent;

    QHostAddress _address;
    int _port;
    int _piecesDownloaded;
    bool *_bitfield;
    QByteArray _protocol;
    QByteArray _reserved;
    QByteArray _infoHash;
    QByteArray _peerId;

    State _state;
    ConnectionInitiator _connectionInitiator;
    bool _amChoking;
    bool _amInterested;
    bool _peerChoking;
    bool _peerInterested;

    QTcpSocket *_socket;
    QByteArray _receivedDataBuffer;
    QTimer _replyTimeoutTimer;
    QTimer _handshakeTimeoutTimer;
    QTimer _reconnectTimer;

    QTimer _sendMessagesTimer;

    bool _hasTimedOut;

    QList<Block *> _blocksQueue;

    bool _isPaused;

	bool readHandshakeReply(bool *ok);

	bool readPeerMessage(bool *ok);

	void connectAll();

	void initBitfield();

	void initClient();

	void initServer(Torrent *torrent, QHostAddress address, int port);

public:
	Peer(ConnectionInitiator connectionInitiator, QTcpSocket *socket);
	~Peer();

	static Peer *createClient(QTcpSocket *socket);

	static Peer* createServer(Torrent *torrent, QHostAddress address, int port);

signals:
	void uploadedData(qint64 bytes);
	void downloadedData(qint64 bytes);

public slots:
	void startConnection();

	void start();
	void pause();

	void sendHandshake();
	void sendChoke();
	void sendUnchoke();
	void sendInterested();
	void sendNotInterested();
	void sendHave(int index);
	void sendBitfield();
	void sendRequest(Block *block);
	void sendPiece(int index, int begin, const QByteArray &blockData);
	void sendCancel(Block *block);

	bool requestBlock();

	void releaseBlock(Block *block);
	void releaseAllBlocks();

	void disconnect();

	void fatalError();

	void sendMessages();

	void connected();
	void readyRead();
	void finished();
	void error(QAbstractSocket::SocketError socketError);
	void replyTimeout();
	void handshakeTimeout();
	void reconnect();
};

#endif
