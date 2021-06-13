#ifndef TORRENTSLIST_H
#define TORRENTSLIST_H

#include "torrentslistitem.h"
#include "panel.h"
#include <QTreeWidget>
#include <QList>

class Torrent;

class TorrentsList : public QTreeWidget
{
	Q_OBJECT

public:
	TorrentsList();
	~TorrentsList();

	TorrentsListItem *torrentItem(Torrent *torrent);
	TorrentsListItem *torrentItem(const QString &name);
	Torrent *currentTorrent();

public slots:
	void addTorrent(Torrent *torrent);
	void removeTorrent(Torrent *torrent);
	void refresh();
	void openContextMenu(const QPoint &pos);
	void openHeaderContextMenu(const QPoint &pos);
	void showAll();
	void showCompleted();
	void showDownloading();
	void showUploading();

signals:
	void removeTorrentSignal(Torrent *torrent, bool deleteData);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
    QList<TorrentsListItem *> _items;
};

#endif
