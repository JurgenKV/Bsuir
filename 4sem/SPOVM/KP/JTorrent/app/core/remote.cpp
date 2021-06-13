#include "remote.h"
#include "jtorrent.h"
#include "ui/mainwindow.h"
#include <QLocalSocket>
#include <QLocalServer>

#define SERVER_NAME ".JTorrent-localServer-socket"
#define TIMEOUT 100

Remote::Remote()
    : _server(new QLocalServer)
    , _socket(nullptr)
{
    connect(_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

Remote::~Remote()
{
    delete _server;
    if (_socket) delete _socket;
}

bool Remote::start()
{
    _socket = new QLocalSocket;
    _socket->connectToServer(SERVER_NAME);
    if (_socket->waitForConnected(TIMEOUT)) {
		sendShowWindow();
		return false;
	}

    delete _socket;
    _socket = nullptr;
	bool serverStarted = false;
    if (!_server->listen(SERVER_NAME)) {
        if (_server->serverError() == QAbstractSocket::AddressInUseError) {
			if (QLocalServer::removeServer(SERVER_NAME)) {
                if (_server->listen(SERVER_NAME)) {
					serverStarted = true;
				}
			}
		}
	} else {
		serverStarted = true;
	}

	return serverStarted;
}

void Remote::newConnection()
{
    if (!_socket) {
        _socket = _server->nextPendingConnection();
        connect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	}
}

void Remote::disconnected()
{
    _socket->deleteLater();
    _buffer.clear();
    _socket = nullptr;
}

void Remote::readyRead()
{
    _buffer.append(_socket->readAll());
	readMessages();
}

void Remote::readMessages()
{
    QList<QByteArray> readData = _buffer.split('\n');
	for (const QByteArray& command : readData) {
		if (command == "0") {
			showWindow();
		}
	}
    int last = _buffer.lastIndexOf('\n');
	if (last == -1) {
		return;
	}
    _buffer.remove(0, last+1);
}

void Remote::sendShowWindow()
{
    _socket->write("0\n");
    _socket->waitForBytesWritten();
}

void Remote::showWindow()
{
    JTorrent::instance()->mainWindow()->show();
}
