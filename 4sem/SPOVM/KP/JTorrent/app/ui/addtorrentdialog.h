#ifndef ADDTORRENTDIALOG_H
#define ADDTORRENTDIALOG_H

#include <QDialog>
#include <QUrl>
#include "core/torrentsettings.h"

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class TorrentInfo;

class AddTorrentDialog : public QDialog
{
	Q_OBJECT

public:
	AddTorrentDialog(QWidget *parent);
	~AddTorrentDialog();

	bool setTorrentUrl(QUrl url);
	bool browseFilePath(QWidget *parent);

	void updateInfo();

public slots:
	void browseDownloadLocation();
	void ok();
	void cancel();

signals:
	void torrentAdded(TorrentInfo* torrentInfo, TorrentSettings settings);

private:
    QLineEdit *_downloadLocation;
    QPushButton *_browseDownloadLocation;
    QCheckBox *_startImmediately;
    QCheckBox *_skipHashCheck;
    QPushButton *_ok;
    QPushButton *_cancel;

    QLabel *_name;
    QLabel *_size;
    QLabel *_infoHash;
    QLabel *_creationDate;
    QLabel *_createdBy;
    QLabel *_comment;

    TorrentInfo *_torrentInfo;

	void connectAll();
	bool loadTorrent(const QString &filePath);
};

#endif
