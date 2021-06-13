#include "torrentmessage.h"
#include <QAbstractSocket>

TorrentMessage::TorrentMessage(Type type)
{
	for (int i = 0; i < 4; i++) {
        _data.push_back((char)0);
	}
    _data.push_back(type);
}

QByteArray &TorrentMessage::getMessage()
{
    int len = _data.size() - 4;
	for (int i = 3; i >= 0; i--) {
        _data[i] = (unsigned char)(len % 256);
		len /= 256;
	}
    return _data;
}

void TorrentMessage::addByte(unsigned char value)
{
    _data.push_back(value);
}

void TorrentMessage::addInt32(qint32 value)
{
	unsigned char bytes[4];
	for (int i = 3; i >= 0; i--) {
		bytes[i] = (unsigned char)(value % 256);
		value /= 256;
	}
	for (int i = 0; i < 4; i++) {
        _data.push_back(bytes[i]);
	}
}

void TorrentMessage::addByteArray(QByteArray value)
{
    _data.push_back(value);
}


void TorrentMessage::keepAlive(QAbstractSocket *socket)
{
	QByteArray arr;
	for (int i = 0; i < 4; i++) {
		arr.push_back((char)0);
	}
	socket->write(arr);
}

void TorrentMessage::choke(QAbstractSocket *socket)
{
	TorrentMessage msg(Choke);
	socket->write(msg.getMessage());
}

void TorrentMessage::unchoke(QAbstractSocket *socket)
{
	TorrentMessage msg(Unchoke);
	socket->write(msg.getMessage());
}

void TorrentMessage::interested(QAbstractSocket *socket)
{
	TorrentMessage msg(Interested);
	socket->write(msg.getMessage());
}

void TorrentMessage::notInterested(QAbstractSocket *socket)
{
	TorrentMessage msg(NotInterested);
	socket->write(msg.getMessage());
}

void TorrentMessage::have(QAbstractSocket *socket, int pieceIndex)
{
	TorrentMessage msg(Have);
	msg.addInt32(pieceIndex);
	socket->write(msg.getMessage());
}

void TorrentMessage::bitfield(QAbstractSocket *socket, const QVector<bool> &bitfield)
{
	TorrentMessage msg(Bitfield);
	unsigned char byte = 0;
	unsigned char pos = (1 << 7);
	for (auto b : bitfield) {
		if (b) {
			byte |= pos;
		}
		pos = pos >> 1;
		if (pos == 0) {
			msg.addByte(byte);
			byte = 0;
			pos = (1 << 7);
		}
	}
	if (pos != (1 << 7)) {
		msg.addByte(byte);
	}
	socket->write(msg.getMessage());
}

void TorrentMessage::request(QAbstractSocket *socket, int index, int begin, int length)
{
	TorrentMessage msg(Request);
	msg.addInt32(index);
	msg.addInt32(begin);
	msg.addInt32(length);
	socket->write(msg.getMessage());
}

void TorrentMessage::piece(QAbstractSocket *socket, int index, int begin, const QByteArray &block)
{
	TorrentMessage msg(Piece);
	msg.addInt32(index);
	msg.addInt32(begin);
	msg.addByteArray(block);
	socket->write(msg.getMessage());
}

void TorrentMessage::cancel(QAbstractSocket *socket, int index, int begin, int length)
{
	TorrentMessage msg(Cancel);
	msg.addInt32(index);
	msg.addInt32(begin);
	msg.addInt32(length);
	socket->write(msg.getMessage());
}

void TorrentMessage::port(QAbstractSocket *socket, int listenPort)
{
	TorrentMessage msg(Port);
	msg.addInt32(listenPort);
	socket->write(msg.getMessage());
}
