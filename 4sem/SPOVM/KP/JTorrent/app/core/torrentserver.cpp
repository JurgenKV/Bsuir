#include "torrentserver.h"
#include "peer.h"
#include <QSettings>
#include <QDebug>

TorrentServer::TorrentServer()
{
    connect(&_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

TorrentServer::~TorrentServer()
{
    disconnect(&_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

bool TorrentServer::startServer()
{
    if (_server.isListening()) {
        _server.close();
	}

	QSettings settings;
	quint64 startPort = settings.value("ServerStartPort", 6881).toInt();
	quint64 endPort = settings.value("ServerEndPort", 6889).toInt();
	settings.setValue("ServerStartPort", startPort);
	settings.setValue("ServerEndPort", endPort);

	if (endPort == 0)
		endPort = startPort;
	if (startPort > 0) {
		for (quint16 port = startPort; port <= endPort; ++port) {
            if (_server.listen(QHostAddress::Any, port)) {
				qDebug() << "Server started on port" << QString::number(port);
				return true;
			}
		}
	}
    if (!_server.listen()) {
        qDebug() << "Failed to start server:" << _server.errorString();
		return false;
	}
    qDebug() << "Server started on port" << QString::number(_server.serverPort());
	return true;
}

void TorrentServer::newConnection()
{
    QTcpSocket *socket = _server.nextPendingConnection();
	Peer *peer = Peer::createClient(socket);
    _peers.push_back(peer);
}

QTcpServer &TorrentServer::server()
{
    return _server;
}

int TorrentServer::port()
{
    return _server.serverPort();
}

QHostAddress TorrentServer::address()
{
    return _server.serverAddress();
}

QList<Peer *> &TorrentServer::peers()
{
    return _peers;
}
