#include "jtorrent.h"
#include "mainwindow.h"
#include "settingswindow.h"
#include "torrentslist.h"
#include "panel.h"
#include "addtorrentdialog.h"
#include "core/torrent.h"
#include "core/torrentinfo.h"
#include "core/torrentmanager.h"
#include <QGuiApplication>
#include <QScreen>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QCloseEvent>
#include <QApplication>
#include <QVBoxLayout>

const int UI_REFRESH_INTERVAL = 300;

MainWindow *MainWindow::_mainWindow = nullptr;

MainWindow::MainWindow()
{
    Q_ASSERT(_mainWindow == nullptr);
    _mainWindow = this;

    _panel = new Panel;
    _settingsWindow = new SettingsWindow;
    _torrentsList = new TorrentsList;

	connect(TorrentManager::instance(), &TorrentManager::torrentAdded,
            _torrentsList, &TorrentsList::addTorrent);
	connect(TorrentManager::instance(), &TorrentManager::torrentRemoved,
            _torrentsList, &TorrentsList::removeTorrent);
	connect(TorrentManager::instance(), &TorrentManager::failedToAddTorrent,
			this, &MainWindow::failedToAddTorrent);
	connect(TorrentManager::instance(), &TorrentManager::failedToResumeTorrents,
			this, &MainWindow::failedToResumeTorrents);
	connect(TorrentManager::instance(), &TorrentManager::error,
			this, &MainWindow::showError);

    connect(_torrentsList, &TorrentsList::removeTorrentSignal,
			TorrentManager::instance(), &TorrentManager::removeTorrent);


    int width = QGuiApplication::primaryScreen()->size().width()*2/4;
    int height = QGuiApplication::primaryScreen()->size().height()*2/4;
	resize(width, height);

    addToolBar(Qt::TopToolBarArea, _panel);

    _stackedWidget = new QStackedWidget;
	QWidget *torrentsListWidget = new QWidget;
	QVBoxLayout *torrentsListLayout = new QVBoxLayout;
    torrentsListLayout->addWidget(_torrentsList);
	torrentsListWidget->setLayout(torrentsListLayout);
    _stackedWidget->addWidget(torrentsListWidget);
    _stackedWidget->addWidget(_settingsWindow);
    setCentralWidget(_stackedWidget);

    connect(_panel, SIGNAL(showAll()), _torrentsList, SLOT(showAll()));
    connect(_panel, SIGNAL(showSettings()), this, SLOT(openSettings()));
    connect(_panel, SIGNAL(hideSettings()), this, SLOT(closeSettings()));

    _panel->openAll();

	createMenus();

    _refreshTimer.setInterval(UI_REFRESH_INTERVAL);
    _refreshTimer.start();
    connect(&_refreshTimer, SIGNAL(timeout()), _torrentsList, SLOT(refresh()));

    QAction *hideClientAction = new QAction(tr("Свернуть"), this);
    connect(hideClientAction, &QAction::triggered, this, &MainWindow::toggleHideShow);

    QAction *exitAction = new QAction(tr("Выход"), this);
	connect(exitAction, &QAction::triggered, this, &MainWindow::exitAction);

    _trayIcon = new QSystemTrayIcon(this);


    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow()
{
}

MainWindow* MainWindow::instance()
{
    Q_ASSERT(_mainWindow != nullptr);
    return _mainWindow;
}

Panel *MainWindow::panel()
{
    return _panel;
}

TorrentsList *MainWindow::torrentsList()
{
    return _torrentsList;
}

void MainWindow::openSettings()
{
    _stackedWidget->setCurrentIndex(1);
}

void MainWindow::closeSettings()
{
    _stackedWidget->setCurrentIndex(0);
}

void MainWindow::createMenus()
{
	menuBar()->show();

    QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));

    QAction *addTorrentAction = new QAction(tr("Добавить торрент файл"), this);
    QAction *exitAction = new QAction(tr("Выход"), this);

	connect(addTorrentAction, &QAction::triggered, this, &MainWindow::addTorrentAction);
	connect(exitAction, &QAction::triggered, this, &MainWindow::exitAction);

	addTorrentAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	exitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

	fileMenu->addAction(addTorrentAction);
	fileMenu->addAction(exitAction);
}


QString MainWindow::getDownloadLocation()
{
	QString downloadPath;
	downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    downloadPath = QFileDialog::getExistingDirectory(this, tr("Выберите путь загрузки"), downloadPath);
	return downloadPath;
}

void MainWindow::failedToAddTorrent(QString errorString)
{
    QMessageBox::critical(this, tr("Ошибка добавления торрента"), errorString);
}

void MainWindow::failedToResumeTorrents(QString errorString)
{
    QMessageBox::critical(this, tr("Ошибка продолжения загрузки"), "Ошибка продолжения загрузки торрент файлов\n\n" + errorString);
}

void MainWindow::showError(QString errorString)
{
    QMessageBox::critical(this, tr("Ошибка"), errorString);
}

void MainWindow::addTorrentAction()
{
	AddTorrentDialog dialog(this);
	connect(&dialog, &AddTorrentDialog::torrentAdded,
			TorrentManager::instance(), &TorrentManager::addTorrentFromInfo);
	if (dialog.browseFilePath(this)) {
		dialog.exec();
	}
}

void MainWindow::exitAction()
{
    if (JTorrent::instance()->question("Подтвердите выход "
									   + QGuiApplication::applicationDisplayName() + "?")) {
		QApplication::quit();
	}
}

void MainWindow::addTorrentFromUrl(QUrl url)
{
	if (url.isLocalFile()) {
		AddTorrentDialog dialog(this);
		connect(&dialog, &AddTorrentDialog::torrentAdded,
				TorrentManager::instance(), &TorrentManager::addTorrentFromInfo);
		if (dialog.setTorrentUrl(url)) {
			dialog.exec();
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
}


void MainWindow::toggleHideShow()
{
	if (isHidden()) {
		show();
	} else {
		hide();
	}
}


void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick) {
		show();
	}
}

void MainWindow::torrentFullyDownloaded(Torrent *torrent)
{
    _trayIcon->showMessage(tr("Торрент загружен "),
                            tr("Торрент %1 был загружен")
							.arg(QString::fromUtf8(torrent->torrentInfo()->torrentName())));
}
