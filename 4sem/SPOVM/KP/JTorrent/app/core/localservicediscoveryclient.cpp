#include "localservicediscoveryclient.h"
#include <QTimer>
#include <QUdpSocket>
#include <QString>
#include <QStringList>

#include "jtorrent.h"
#include "torrentserver.h"
#include "torrent.h"
#include "torrentinfo.h"

LocalServiceDiscoveryClient::LocalServiceDiscoveryClient(QObject *parent)
	: QObject(parent)
{
    _announceTimer = new QTimer(this);
    _socketIPv4 = new QUdpSocket(this);
    _socketIPv4->bind(QHostAddress::AnyIPv4, LSD_PORT_IPV4, QUdpSocket::ShareAddress);
    _socketIPv4->joinMulticastGroup(QHostAddress(LSD_ADDRESS_IPV4));
    _socketIPv4->setSocketOption(QAbstractSocket::MulticastTtlOption, 20);

    _socketIPv6 = new QUdpSocket(this);
    _socketIPv6->bind(QHostAddress::AnyIPv6, LSD_PORT_IPV6, QUdpSocket::ShareAddress);
    _socketIPv6->joinMulticastGroup(QHostAddress(LSD_ADDRESS_IPV6));
    _socketIPv6->setSocketOption(QAbstractSocket::MulticastTtlOption, 20);
	for (int i = 0; i < 20; i++) {
		int q = qrand() % (10+26+26);
		if (q < 10) {
            _cookie.append('0' + q);
		} else if (q < 36) {
            _cookie.append('A' + q - 10);
		} else {
            _cookie.append('a' + q - 36);
		}
	}

    connect(_announceTimer, &QTimer::timeout, this, &LocalServiceDiscoveryClient::announceAll);
    connect(_socketIPv4, &QUdpSocket::readyRead, this, &LocalServiceDiscoveryClient::processPendingDatagrams);
    connect(_socketIPv6, &QUdpSocket::readyRead, this, &LocalServiceDiscoveryClient::processPendingDatagrams);
}

void LocalServiceDiscoveryClient::announce(QUdpSocket *socket, const char *address, int port)
{
	QHostAddress addr(address);
	QString addressString = address;
	if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
		addressString.prepend('[');
		addressString.append(']');
	}
	QString datagramString;
	QTextStream datagramStream(&datagramString);
	datagramStream << "BT-SEARCH * HTTP/1.1\r\n"
				   << "Host: " << addressString << ":" << port << "\r\n"
                   << "Port: " << JTorrent::instance()->server()->port() << "\r\n";

    for (Torrent *torrent : JTorrent::instance()->torrents()) {
		QByteArray hash = torrent->torrentInfo()->infoHash().toHex().toLower();
		datagramStream << "Infohash: " << hash << "\r\n";
	}

    datagramStream << "cookie: " << _cookie << "\r\n";
	datagramStream << "\r\n\r\n";
	socket->writeDatagram(datagramString.toLatin1(), addr, port);

    _announceTimer->start(LSD_INTERVAL);
    _elapsedTimer.start();
}

void LocalServiceDiscoveryClient::announceAll()
{
    int elapsed = _elapsedTimer.elapsed();
    if (elapsed < LSD_MIN_INTERVAL && _announceTimer->isActive()) {
        _announceTimer->start(LSD_MIN_INTERVAL - elapsed);
		return;
	}

    if (JTorrent::instance()->torrents().isEmpty()) {
		return;
	}

	announceIPv4();
	announceIPv6();
}

void LocalServiceDiscoveryClient::announceIPv4()
{
    announce(_socketIPv4, LSD_ADDRESS_IPV4, LSD_PORT_IPV4);
}

void LocalServiceDiscoveryClient::announceIPv6()
{
    announce(_socketIPv6, LSD_ADDRESS_IPV6, LSD_PORT_IPV6);
}

void LocalServiceDiscoveryClient::processPendingDatagrams()
{
	QList<QByteArray> infoHashes;
    for (Torrent *torrent : JTorrent::instance()->torrents()) {
		infoHashes.append(torrent->torrentInfo()->infoHash().toHex().toLower());
	}

	for (;;) {
		QUdpSocket *senderSocket = nullptr;
        if (_socketIPv4->hasPendingDatagrams()) {
            senderSocket = _socketIPv4;
        } else if (_socketIPv6->hasPendingDatagrams()) {
            senderSocket = _socketIPv6;
		} else {
			break;
		}

		QHostAddress sender;
		QByteArray datagram;
		datagram.resize(senderSocket->pendingDatagramSize());
		senderSocket->readDatagram(datagram.data(), datagram.size(), &sender);

		int port = 0;
		QList<QByteArray> receivedInfoHashes;
		QByteArray cookie;

		QTextStream stream(datagram);
		QString line;
		stream.readLineInto(&line);
		if (line != "BT-SEARCH * HTTP/1.1") {
			continue;
		}
		while (stream.readLineInto(&line)) {
			QStringList splitLine = line.split(": ");
			if (splitLine.size() == 2) {
				QString header = splitLine.first();
				if (header == "Host") {
				} else if (header == "Port") {
					bool ok;
					port = splitLine.last().toInt(&ok);
					if(!ok) {
						port = 0;
					}
				} else if (header == "Infohash") {
					receivedInfoHashes.append(splitLine.last().toLatin1().toLower());
				} else if (header == "cookie") {
					cookie = splitLine.last().toLatin1();
				}
			}
		}

        if (port == 0 || cookie == _cookie) {
			continue;
		}

		for (QByteArray& hash : receivedInfoHashes) {
			if (infoHashes.contains(hash)) {
                for (Torrent *torrent : JTorrent::instance()->torrents()) {
					if (torrent->torrentInfo()->infoHash().toHex().toLower() == hash) {
						emit foundPeer(sender, port, torrent);
					}
				}
			}
		}
	}
}
