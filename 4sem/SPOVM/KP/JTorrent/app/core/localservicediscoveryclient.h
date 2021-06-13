#ifndef LOCALSERVICEDISCOVERY_H
#define LOCALSERVICEDISCOVERY_H

#define LSD_ADDRESS_IPV4 "37.214.31.191"
#define LSD_PORT_IPV4 6771
#define LSD_ADDRESS_IPV6 "ff15::efc0:988f" //ff15::efc0:988f
#define LSD_PORT_IPV6 6771
#define LSD_INTERVAL 5*60*1000
#define LSD_MIN_INTERVAL 60*1000

#include <QObject>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QByteArray>

class Torrent;
class QTimer;
class QUdpSocket;

class LocalServiceDiscoveryClient : public QObject
{
	Q_OBJECT

public:
	LocalServiceDiscoveryClient(QObject *parent = nullptr);

public slots:
	void announceAll();
	void announceIPv4();
	void announceIPv6();
	void processPendingDatagrams();

signals:
	void foundPeer(QHostAddress address, int port, Torrent *torrent);

private:
    QTimer *_announceTimer;
    QElapsedTimer _elapsedTimer;
    QUdpSocket *_socketIPv4;
    QUdpSocket *_socketIPv6;
    QByteArray _cookie;

	void announce(QUdpSocket *socket, const char *address, int port);
};

#endif
