#ifndef TORRENTMANAGER_H
#define TORRENTMANAGER_H

#include "torrentsettings.h"
#include <QObject>
#include <QList>
#include <QUrl>

class Torrent;
class TorrentInfo;

class TorrentManager : public QObject
{
	Q_OBJECT

public:
	TorrentManager();
	~TorrentManager();

	static TorrentManager* instance();

	const QList<Torrent *> &torrents() const;

signals:
	void torrentAdded(Torrent *torrent);
	void torrentRemoved(Torrent *torrent);
	void failedToAddTorrent(QString errorString);
	void failedToResumeTorrents(QString errorString);
	void error(QString errorString);

public slots:
	void resumeTorrents();

	void addTorrentFromInfo(TorrentInfo *torrentInfo, const TorrentSettings &settings);

	void saveTorrentsResumeInfo();
	bool saveTorrentFile(const QString &filename, TorrentInfo *torrentInfo);

	bool removeTorrent(Torrent *torrent, bool deleteData);


private:
    QList<Torrent *> _torrents;

    static TorrentManager *_torrentManager;
};

#endif
