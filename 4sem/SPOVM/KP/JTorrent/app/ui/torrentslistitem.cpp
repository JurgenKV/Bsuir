#include "torrentslistitem.h"
#include "jtorrent.h"
#include "mainwindow.h"
#include "panel.h"
#include "core/torrent.h"
#include "core/torrentinfo.h"
#include "core/trafficmonitor.h"
#include "global.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QFileInfo>
#include <QFile>
#include <QDesktopServices>

TorrentsListItem::TorrentsListItem(QTreeWidget *view, Torrent *torrent)
	: QTreeWidgetItem(view)
    , _torrent(torrent)
{
}

void TorrentsListItem::setSortData(int column, QVariant data)
{
    _sortData[column] = data;
}

bool TorrentsListItem::operator<(const QTreeWidgetItem &other) const
{
	int column = treeWidget()->sortColumn();
	const TorrentsListItem *otherCasted = dynamic_cast<const TorrentsListItem *>(&other);
	if (otherCasted) {
        if (_sortData.contains(column) && otherCasted->_sortData.contains(column)) {
            return _sortData[column] < otherCasted->_sortData[column];
		}
	}
	return text(column) < other.text(column);
}

void TorrentsListItem::refresh()
{
	Torrent *t = torrent();
	TorrentInfo *info = t->torrentInfo();
	setName(info->torrentName());
	setSize(info->length());
	setPeers(t->connectedPeersCount(), t->allPeersCount());
	setState(t->stateString());
	setProgress(t->percentDownloaded());
	setDownloadSpeed(t->trafficMonitor()->downloadSpeed());
	setLeft(t->bytesLeft());
	setTotalDownloaded(t->totalBytesDownloaded());
	float ratio = (double)t->totalBytesUploaded() / (double)t->totalBytesDownloaded();
	if (t->totalBytesDownloaded() == 0) {
		ratio = 0.0f;
	}
	setHidden(!belongsToSection());
}

bool TorrentsListItem::belongsToSection()
{
    Panel::Section section = JTorrent::instance()->mainWindow()->panel()->getCurrentSection();
	switch (section) {
	case Panel::All:
		return true;
	case Panel::Settings:
		return false;
	}
	Q_ASSERT(false);
	return true;
}

Torrent *TorrentsListItem::torrent() const
{
    return _torrent;
}


void TorrentsListItem::setName(const QString &value)
{
	setText(Name, value);
}

void TorrentsListItem::setSize(qint64 value)
{
	setText(Size, formatSize(value));
	setSortData(Size, value);
}

void TorrentsListItem::setPeers(int connected, int all)
{
	setText(Peers, tr("%1/%2").arg(connected).arg(all));
	setSortData(Peers, connected);
}

void TorrentsListItem::setState(const QString &state)
{
	setText(State, state);
	setSortData(State, state);
}

void TorrentsListItem::setProgress(float value)
{
	setText(Progress, QString::number(value, 'f', 2) + "%");
	setSortData(Progress, value);
}

void TorrentsListItem::setDownloadSpeed(qint64 bytes)
{
    setText(DownloadSpeed, formatSize(bytes) + "/с");
	setSortData(DownloadSpeed, bytes);
}

void TorrentsListItem::setLeft(qint64 value)
{
	setText(Left, formatSize(value));
	setSortData(Left, value);
}

void TorrentsListItem::setTotalDownloaded(qint64 value)
{
	setText(TotalDownloaded, formatSize(value));
	setSortData(TotalDownloaded, value);
}


void TorrentsListItem::onOpenAction()
{
    QFileInfo info(*_torrent->files().first());
	QUrl url = QUrl::fromLocalFile(info.absoluteFilePath());
	QDesktopServices::openUrl(url);
}

void TorrentsListItem::onOpenLocationAction()
{
    QUrl url = QUrl::fromLocalFile(_torrent->downloadLocation());
	QDesktopServices::openUrl(url);
}

void TorrentsListItem::onPauseAction()
{
    _torrent->pause();
}

void TorrentsListItem::onStartAction()
{
    _torrent->start();
}

void TorrentsListItem::onStopAction()
{
    _torrent->stop();
}

void TorrentsListItem::onRecheckAction()
{
    _torrent->check();
}

void TorrentsListItem::onRemoveAction()
{
    QDialog dialog(JTorrent::instance()->mainWindow());
	QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("Вы уверены, что хотите сделать это?")));
    QCheckBox *deleteData = new QCheckBox("Удаление загруженных данных");
    deleteData->setVisible(0);
    layout->addWidget(deleteData);
	QHBoxLayout *bottomLayout = new QHBoxLayout;
    QPushButton *yes = new QPushButton("Да");
    QPushButton *no = new QPushButton("Нет");
	connect(yes, SIGNAL(clicked()), &dialog, SLOT(accept()));
	connect(no, SIGNAL(clicked()), &dialog, SLOT(reject()));
	bottomLayout->addWidget(yes);
	bottomLayout->addWidget(no);
	layout->addLayout(bottomLayout);
	dialog.setLayout(layout);
	if (dialog.exec()) {
        emit removeTorrent(_torrent, deleteData->isChecked());
	}
}
