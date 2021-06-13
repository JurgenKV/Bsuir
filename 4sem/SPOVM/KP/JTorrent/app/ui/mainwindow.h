#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QUrl>

class Panel;
class QStackedWidget;
class TorrentsList;
class SettingsWindow;
class TorrentInfoPanel;
class Torrent;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

	static MainWindow* instance();

	Panel *panel();
	TorrentsList *torrentsList();

	void closeEvent(QCloseEvent *event);

	QString getDownloadLocation();

private:
    Panel *_panel;
    QStackedWidget *_stackedWidget;
    SettingsWindow *_settingsWindow;
    TorrentsList *_torrentsList;
    TorrentInfoPanel *_infoPanel;

    QMenu *_trayIconMenu;
    QSystemTrayIcon *_trayIcon;

    QTimer _refreshTimer;

    static MainWindow *_mainWindow;

	void createMenus();

public slots:
	void failedToAddTorrent(QString errorString);
	void failedToResumeTorrents(QString errorString);
	void showError(QString errorString);
	void openSettings();
	void closeSettings();
	void addTorrentAction();
	void exitAction();
	void toggleHideShowTorrentInfoPanel();
	void aboutAction();
	void toggleHideShow();
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void torrentFullyDownloaded(Torrent *torrent);
	void addTorrentFromUrl(QUrl url);
};

#endif
