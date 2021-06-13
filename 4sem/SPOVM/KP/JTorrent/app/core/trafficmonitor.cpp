#include "trafficmonitor.h"
#include "peer.h"

#define MONITOR_INTERVAL 1000

TrafficMonitor::TrafficMonitor(QObject *parent)
	: QObject(parent)
    , _uploadSpeed(0)
    , _downloadSpeed(0)
    , _bytesUploaded(0)
    , _bytesDownloaded(0)
{
    _timer.start(MONITOR_INTERVAL);
    connect(&_timer, &QTimer::timeout, this, &TrafficMonitor::update);
}

qint64 TrafficMonitor::uploadSpeed() const
{
    return _uploadSpeed;
}

qint64 TrafficMonitor::downloadSpeed() const
{
    return _downloadSpeed;
}

void TrafficMonitor::addPeer(Peer *peer)
{
	connect(peer, &Peer::uploadedData, this, &TrafficMonitor::onDataSent);
	connect(peer, &Peer::downloadedData, this, &TrafficMonitor::onDataReceived);
}

void TrafficMonitor::removePeer(Peer *peer)
{
	disconnect(peer, &Peer::uploadedData, this, &TrafficMonitor::onDataSent);
	disconnect(peer, &Peer::downloadedData, this, &TrafficMonitor::onDataReceived);
}

void TrafficMonitor::onDataSent(qint64 bytes)
{
    _bytesUploaded += bytes;
}

void TrafficMonitor::onDataReceived(qint64 bytes)
{
    _bytesDownloaded += bytes;
}

void TrafficMonitor::update()
{
    _uploadSpeed = _bytesUploaded / (MONITOR_INTERVAL / 1000.0);
    _downloadSpeed = _bytesDownloaded / (MONITOR_INTERVAL / 1000.0);
    _bytesUploaded = 0;
    _bytesDownloaded = 0;
    emit uploadSpeedChanged(_uploadSpeed);
    emit downloadSpeedChanged(_downloadSpeed);
}
