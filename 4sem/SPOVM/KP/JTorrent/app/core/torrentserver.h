#ifndef TORRENTSERVER_H
#define TORRENTSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QList>

class Peer;

class TorrentServer : public QObject
{
	Q_OBJECT

public:
	TorrentServer();
	~TorrentServer();

	bool startServer();

	QTcpServer& server();
	int port();
	QHostAddress address();
	QList<Peer *> &peers();

public slots:
	void newConnection();

private:
    QTcpServer _server;
    QList<Peer *> _peers;
};

#endif
