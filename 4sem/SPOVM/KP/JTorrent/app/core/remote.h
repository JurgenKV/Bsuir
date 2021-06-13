#ifndef REMOTE_H
#define REMOTE_H

#include <QObject>

class QLocalSocket;
class QLocalServer;

class Remote : public QObject
{
	Q_OBJECT

public:
	Remote();
	~Remote();

	bool start();

	void sendShowWindow();
	void showWindow();

public slots:
	void newConnection();
	void disconnected();
	void readyRead();

private:
    QLocalServer *_server;
    QLocalSocket *_socket;
    QByteArray _buffer;

	void readMessages();
};

#endif
