#include "jtorrent.h"
#include "core/torrent.h"
#include "core/torrentinfo.h"
#include "core/torrentmanager.h"
#include "core/torrentserver.h"
#include "core/localservicediscoveryclient.h"
#include "core/trackerclient.h"
#include "ui/mainwindow.h"
#include <QGuiApplication>
#include <QMessageBox>
#include <QUrlQuery>

JTorrent *JTorrent::_instance;

JTorrent::JTorrent()
{
	qsrand(QDateTime::currentMSecsSinceEpoch());

    _instance = this;

    _torrentManager = new TorrentManager;
    _server = new TorrentServer;
    _LSDClient = new LocalServiceDiscoveryClient;
    _mainWindow = new MainWindow;

    _peerId.push_back("JT");
    while (_peerId.size() < 20) {
        _peerId.push_back(char(qrand() % 256));
	}

    connect(_LSDClient, &LocalServiceDiscoveryClient::foundPeer, this, &JTorrent::LSDPeerFound);

	startServer();
    _torrentManager->resumeTorrents();
	startLSDClient();
	showMainWindow();
}

JTorrent::~JTorrent()
{
    delete _torrentManager;
    delete _server;
    delete _LSDClient;
    delete _mainWindow;
}


bool JTorrent::startServer()
{
    return _server->startServer();
}

void JTorrent::startLSDClient()
{
    _LSDClient->announceAll();
    connect(_torrentManager, &TorrentManager::torrentAdded, _LSDClient, &LocalServiceDiscoveryClient::announceAll);
}

void JTorrent::shutDown()
{
	for (Torrent *torrent : torrents()) {
		torrent->stop();
	}
    _torrentManager->saveTorrentsResumeInfo();
}

void JTorrent::showMainWindow()
{
    _mainWindow->show();
}

void JTorrent::critical(const QString &text)
{
    QMessageBox::critical(_mainWindow, QGuiApplication::applicationDisplayName(), text);
}

void JTorrent::information(const QString &text)
{
    QMessageBox::information(_mainWindow, QGuiApplication::applicationDisplayName(), text);
}

bool JTorrent::question(const QString &text)
{
	QMessageBox::StandardButton ans;
    ans = QMessageBox::question(_mainWindow, QGuiApplication::applicationDisplayName(), text);
	return ans == QMessageBox::Yes;
}

void JTorrent::warning(const QString &text)
{
    QMessageBox::warning(_mainWindow, QGuiApplication::applicationDisplayName(), text);
}


const QByteArray &JTorrent::peerId() const
{
    return _peerId;
}


const QList<Torrent *> &JTorrent::torrents() const
{
    return _torrentManager->torrents();
}

TorrentManager *JTorrent::torrentManager()
{
    return _torrentManager;
}

TorrentServer *JTorrent::server()
{
    return _server;
}


MainWindow *JTorrent::mainWindow()
{
    return _mainWindow;
}

JTorrent *JTorrent::instance()
{
    return _instance;
}


void JTorrent::LSDPeerFound(QHostAddress address, int port, Torrent *torrent)
{
	torrent->connectToPeer(address, port);
}
