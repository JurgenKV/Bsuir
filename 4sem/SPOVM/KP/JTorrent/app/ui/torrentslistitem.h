#ifndef TORRENTSLISTITE_H
#define TORRENTSLISTITE_H

#include <QTreeWidgetItem>
#include <QVariant>
#include <QMap>

class Torrent;

class TorrentsListItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT

public:
	enum Section {
        Name=0, Size, Peers, State, Progress, DownloadSpeed,
        Left, TotalDownloaded
	};

	TorrentsListItem(QTreeWidget *view, Torrent *torrent);

	void setSortData(int column, QVariant data);
	bool operator<(const QTreeWidgetItem &other) const;
	void refresh();
	bool belongsToSection();

	Torrent *torrent() const;

	void setName(const QString &value);
	void setSize(qint64 value);
	void setPeers(int connected, int all);
	void setState(const QString &state);
	void setProgress(float value);
	void setDownloadSpeed(qint64 bytes);
	void setLeft(qint64 value);
	void setTotalDownloaded(qint64 value);
	void setDownloaded(qint64 value);

public slots:
	void onOpenAction();
	void onOpenLocationAction();
	void onPauseAction();
	void onStartAction();
	void onStopAction();
	void onRecheckAction();
	void onRemoveAction();

signals:
	void removeTorrent(Torrent *torrent, bool deleteData);

private:
    Torrent *_torrent;
    QMap<int, QVariant> _sortData;
};

#endif
