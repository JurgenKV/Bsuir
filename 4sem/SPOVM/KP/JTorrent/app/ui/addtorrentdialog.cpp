#include "addtorrentdialog.h"
#include "jtorrent.h"
#include "core/torrentinfo.h"
#include "core/torrentmanager.h"
#include "global.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>

AddTorrentDialog::AddTorrentDialog(QWidget *parent)
	: QDialog(parent)
    , _torrentInfo(nullptr)
{
	QVBoxLayout *layout = new QVBoxLayout;

	QGridLayout *settingsLayout = new QGridLayout;
    settingsLayout->addWidget(_startImmediately = new QCheckBox("Быстрый старт"), 0, 0);
    settingsLayout->addWidget(_skipHashCheck = new QCheckBox("Просмотреть хэш"), 1, 0);
    _startImmediately->setChecked(true);
    _skipHashCheck->setChecked(false);

    QGroupBox *infoBox = new QGroupBox("Информация о торрент файле");



	QGridLayout *infoLayout = new QGridLayout;

    infoLayout->addWidget(new QLabel("Название: "), 0, 0, 1, 1);
    infoLayout->addWidget(_name = new QLabel(), 0, 1, 1, 1);
    infoLayout->addWidget(new QLabel("Размер: "), 1, 0, 1, 1);
    infoLayout->addWidget(_size = new QLabel(), 1, 1, 1, 1);
    infoLayout->addWidget(new QLabel("Хэш информация: "), 2, 0, 1, 1);
    infoLayout->addWidget(_infoHash = new QLabel(), 2, 1, 1, 1);
    infoLayout->addWidget(new QLabel("Дата создания: "), 3, 0, 1, 1);
    infoLayout->addWidget(_creationDate = new QLabel(), 3, 1, 1, 1);
    infoLayout->addWidget(new QLabel("Создатель: "), 4, 0 , 1, 1);
    infoLayout->addWidget(_createdBy = new QLabel(), 4, 1, 1, 1);
    infoLayout->addWidget(new QLabel("Комментарий: "), 5, 0, 1, 1);
    infoLayout->addWidget(_comment = new QLabel(), 5, 1, 1, 1);

	infoBox->setLayout(infoLayout);
	layout->addWidget(infoBox);


    QGroupBox *downloadLocationBox = new QGroupBox("Директория загрузки");
    QHBoxLayout *downloadLocationLayout = new QHBoxLayout;
    downloadLocationLayout->addWidget(_downloadLocation = new QLineEdit);
    downloadLocationLayout->addWidget(_browseDownloadLocation= new QPushButton("Выбрать"));
    downloadLocationBox->setLayout(downloadLocationLayout);
    layout->addWidget(downloadLocationBox);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(_ok = new QPushButton("Ок"), 0, Qt::AlignRight);
    bottomLayout->addWidget(_cancel = new QPushButton("Отмена"), Qt::AlignRight);
    _ok->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    _cancel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    layout->addLayout(bottomLayout);

    setLayout(layout);

	int screenWidth = QGuiApplication::primaryScreen()->size().width();
	setFixedWidth(screenWidth / 3);

	connectAll();

    _downloadLocation->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

AddTorrentDialog::~AddTorrentDialog()
{
    if (_torrentInfo) {
        delete _torrentInfo;
	}
}

void AddTorrentDialog::connectAll()
{
    connect(_browseDownloadLocation, SIGNAL(clicked()), this, SLOT(browseDownloadLocation()));
    connect(_ok, SIGNAL(clicked()), this, SLOT(ok()));
    connect(_cancel, SIGNAL(clicked()), this, SLOT(cancel()));
}

bool AddTorrentDialog::setTorrentUrl(QUrl url)
{
	if (url.isLocalFile()) {
		QString filePath = url.toLocalFile();

        if (_torrentInfo) {
            delete _torrentInfo;
            _torrentInfo = nullptr;
		}

		if (loadTorrent(filePath)) {
			updateInfo();
			return true;
		}
	}
	return false;
}

bool AddTorrentDialog::browseFilePath(QWidget *parent)
{
	QString filePath;
	for (;;) {

        if (_torrentInfo) {
            delete _torrentInfo;
            _torrentInfo = nullptr;
		}

        filePath = QFileDialog::getOpenFileName(parent, tr("Открыть торрент "),
                                                QDir::homePath(), tr("Торрент файл (*.torrent)"));
		if (filePath.isEmpty()) {
			return false;
		}
		if (loadTorrent(filePath)) {
			updateInfo();
			break;
		}
	}
	return true;
}

void AddTorrentDialog::browseDownloadLocation()
{
    QString downloadLocation = _downloadLocation->text();
    downloadLocation = QFileDialog::getExistingDirectory(this, tr("Выбор директории загрузки"),
					   downloadLocation, QFileDialog::ShowDirsOnly);

	if (downloadLocation.isEmpty()) {
		return;
	}

    _downloadLocation->setText(downloadLocation);
}

bool AddTorrentDialog::loadTorrent(const QString &filePath)
{
	if (!filePath.endsWith(".torrent", Qt::CaseInsensitive)) {
        QMessageBox::warning(this, tr("Добавление торрент файла"),
                             tr("Неверный формат файла"));
		return false;
	}
	if (!QFile::exists(filePath)) {
        QMessageBox::warning(this, tr("Добавление торрент файла"),
                             tr("Файл '%1' не найден")
							 .arg(filePath));
		return false;
	}

    if (_torrentInfo) {
        delete _torrentInfo;
	}

    _torrentInfo = new TorrentInfo;
    if (!_torrentInfo->loadFromTorrentFile(filePath)) {
        QMessageBox::warning(this, tr("Добавление торрент файла"),
                             tr("Ошибка добавления торрента \n\nПричина: %1")
                             .arg(_torrentInfo->errorString()));
		return false;
	}

    setWindowTitle(_torrentInfo->torrentName());
	show();
	return true;
}

void AddTorrentDialog::ok()
{
    if (!_torrentInfo) {
        QMessageBox::warning(this, tr("Добавление торрент файла"),
                             tr("Выберите торрент файл"));
		return;
	}

    QString downloadLocation = _downloadLocation->text();
	QDir dir(downloadLocation);
	if (!dir.exists()) {
        QMessageBox::warning(this, tr("Добавление торрент файла"),
                             tr("Выберите нормальный путь"));
		return;
	}

	TorrentSettings settings;
	settings.setDownloadLocation(downloadLocation);
    settings.setStartImmediately(_startImmediately->isChecked());
    settings.setSkipHashCheck(_skipHashCheck->isChecked());

    emit torrentAdded(_torrentInfo, settings);
    _torrentInfo = nullptr;
	close();
}

void AddTorrentDialog::cancel()
{
    if (_torrentInfo) {
        delete _torrentInfo;
        _torrentInfo = nullptr;
	}
	close();
}

void AddTorrentDialog::updateInfo()
{
    if (_torrentInfo) {
        _name->setText(_torrentInfo->torrentName());
        _size->setText(tr("%1 (%2 bytes)")
                        .arg(formatSize(_torrentInfo->length()))
                        .arg(QString::number(_torrentInfo->length())));
        _infoHash->setText(_torrentInfo->infoHash().toHex());
        _creationDate->setText(_torrentInfo->creationDate() ? _torrentInfo->creationDate()->toString() : "N/A");
        _createdBy->setText(_torrentInfo->createdBy() ? *_torrentInfo->createdBy() : "N/A");
        _comment->setText(_torrentInfo->comment() ? *_torrentInfo->comment() : "N/A");
	} else {
        _name->clear();
        _size->clear();
        _infoHash->clear();
        _creationDate->clear();
        _createdBy->clear();
        _comment->clear();
	}
}
