#ifndef JTorrent_H
#define JTorrent_H

#include "core/torrentsettings.h"
#include <QObject>
#include <QHostAddress>
#include <QList>
#include <QString>
#include <QUrl>

class Torrent;
class TorrentManager;
class TorrentServer;
class MainWindow;
class LocalServiceDiscoveryClient;

class JTorrent : public QObject
{
	Q_OBJECT

public:
    JTorrent();
    ~JTorrent();

	bool startServer();
	void startLSDClient();

	void shutDown();

	void showMainWindow();

	void critical(const QString &text);
	void information(const QString &text);
	bool question(const QString &text);
	void warning(const QString &text);

	const QByteArray &peerId() const;
	const QList<Torrent *> &torrents() const;
	TorrentManager *torrentManager();
	TorrentServer *server();
	MainWindow *mainWindow();

    static JTorrent *instance();

public slots:
	void LSDPeerFound(QHostAddress address, int port, Torrent *torrent);

private:
    QByteArray _peerId;

    TorrentManager *_torrentManager;
    TorrentServer *_server;
    LocalServiceDiscoveryClient *_LSDClient;

    MainWindow *_mainWindow;

    static JTorrent *_instance;
};

#endif
