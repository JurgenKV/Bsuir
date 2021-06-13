#include "torrentmanager.h"
#include "torrentinfo.h"
#include "torrent.h"
#include "resumeinfo.h"
#include "bencodeparser.h"
#include "ui/mainwindow.h"
#include "jtorrent.h"
#include <QDir>
#include <QStandardPaths>

TorrentManager *TorrentManager::_torrentManager = nullptr;


TorrentManager::TorrentManager()
{
    Q_ASSERT(_torrentManager == nullptr);
    _torrentManager = this;
}

TorrentManager::~TorrentManager()
{
    for (Torrent *torrent : _torrents) {
		delete torrent;
	}
}

TorrentManager* TorrentManager::instance()
{
    Q_ASSERT(_torrentManager != nullptr);
    return _torrentManager;
}

void TorrentManager::addTorrentFromInfo(TorrentInfo *torrentInfo, const TorrentSettings &settings)
{
    for (Torrent *t : _torrents) {
		if (t->torrentInfo()->infoHash() == torrentInfo->infoHash()) {
            emit failedToAddTorrent("Данный торрент уже был добавлен");
			delete torrentInfo;
			return;
		}
	}

	Torrent *torrent = new Torrent();
	if (!torrent->createNew(torrentInfo, settings.downloadLocation())) {
        emit failedToAddTorrent("Ошибка добавления торрента: " + torrent->errorString());
		torrent->deleteLater();
		return;
	}

	if (!saveTorrentFile(torrentInfo->creationFileName(), torrentInfo)) {
        emit failedToAddTorrent("Ошибка сохранения торрента");
		torrent->deleteLater();
		return;
	}

    _torrents.push_back(torrent);

	if (!settings.skipHashCheck()) {
		torrent->check();
	}

	if (settings.startImmediately()) {
		torrent->start();
	} else {
		torrent->pause();
	}

	emit torrentAdded(torrent);

	saveTorrentsResumeInfo();
}

void TorrentManager::resumeTorrents()
{
	QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(dataPath);
	if (!dir.exists()) {
		if (!dir.mkpath(dataPath)) {
            emit failedToResumeTorrents("Невозможно создать путь директории " + dataPath);
			return;
		}
	}
	if (!dir.exists("resume")) {
		if (!dir.mkdir("resume")) {
            emit failedToResumeTorrents("Невозможно создать директорию " + dataPath + "/resume");
			return;
		}
	}
	dir.cd("resume");
	QFile resumeFile(dir.path() + "/resume.dat");
	if (!resumeFile.exists()) {
		return;
	}
	if (!resumeFile.open(QIODevice::ReadOnly)) {
		QFileInfo info(resumeFile);
        emit failedToResumeTorrents("Ошибка открытия файла " + info.absoluteFilePath() + ": " + resumeFile.errorString());
		return;
	}
	QByteArray resumeData = resumeFile.readAll();
	BencodeParser parser;
	try {
		if (!parser.parse(resumeData)) {
			throw BencodeException(parser.errorString());
		}
		if (parser.list().isEmpty()) {
			throw BencodeException("Main bencode list is empty");
		}
		if (parser.list().size() != 1) {
			throw BencodeException("Main bencode list has size of "
								   + QString::number(parser.list().size()) + ", expected 1");
		}

		BencodeDictionary *mainDict = parser.list().first()->toBencodeDictionary();
		for (QByteArray infoHash : mainDict->keys()) {
			QFile file(dir.path() + "/" + infoHash.toHex() + ".torrent");
			if (!file.exists()) {
				QFileInfo info(file);
				qDebug() << "TorrentManager::resumeTorrents(): file" << info.absoluteFilePath() << "Not found";
				continue;
			}

			TorrentInfo *torrentInfo = new TorrentInfo;
			if (!torrentInfo->loadFromTorrentFile(file.fileName())) {
				qDebug() << "TorrentManager::resumeTorrents(): Failed to parse" << file.fileName()
						 << torrentInfo->errorString();
				delete torrentInfo;
				continue;
			}

			ResumeInfo resumeInfo(torrentInfo);

			BencodeValue *value = mainDict->value(infoHash);
			if (!value->isDictionary()) {
				qDebug() << "TorrentManager::resumeTorrents(): Failed to parse" << file.fileName()
						 << ": value for infohash is not a dictionary";
				delete torrentInfo;
				continue;
			}
			if (!resumeInfo.loadFromBencode(value->toBencodeDictionary())) {
				qDebug() << "TorrentManager::resumeTorrents(): Failed to load resume info for" << file.fileName();
				delete torrentInfo;
				continue;
			}

			Torrent *torrent = new Torrent();
			if (!torrent->createFromResumeInfo(torrentInfo, &resumeInfo)) {
				qDebug() << "TorrentManager::resumeTorrents(): Failed to create torrent from resume data for"
						 << file.fileName() << torrent->errorString();
				torrent->deleteLater();
				continue;
			}

            _torrents.push_back(torrent);
			emit torrentAdded(torrent);

		}

	} catch (BencodeException &ex) {
		emit failedToResumeTorrents("Failed to read resume file: " + ex.what());
		return;
	}
}

void TorrentManager::saveTorrentsResumeInfo()
{
	QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(dataPath);
	if (!dir.exists()) {
		if (!dir.mkpath(dataPath)) {
			emit error("Failed to create directory " + dataPath);
			return;
		}
	}
	if (!dir.exists("resume")) {
		if (!dir.mkdir("resume")) {
			emit error("Failed to create directory " + dataPath + "/resume");
			return;
		}
	}
	dir.cd("resume");
	QFile resumeFile(dir.path() + "/resume.dat");
	if (!resumeFile.open(QIODevice::WriteOnly)) {
		QFileInfo info(resumeFile);
		emit error("Failed to open " + info.absoluteFilePath() + ": " + resumeFile.errorString());
		return;
	}
	BencodeDictionary *mainDict = new BencodeDictionary;
    for (Torrent *torrent : _torrents) {
		torrent->getResumeInfo().addToBencode(mainDict);
	}
	QByteArray bencodedData = mainDict->bencode(true);
	resumeFile.write(bencodedData);
	resumeFile.close();
	delete mainDict;
}

bool TorrentManager::saveTorrentFile(const QString &filename, TorrentInfo *torrentInfo)
{
	QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(dataPath);
	if (!dir.exists()) {
		if (!dir.mkpath(dataPath)) {
			return false;
		}
	}
	if (!dir.exists("resume")) {
		if (!dir.mkdir("resume")) {
			return false;
		}
	}
	dir.cd("resume");

	QString newTorrentPath = dir.absoluteFilePath(torrentInfo->infoHash().toHex() + ".torrent");
	if (QFile::exists(newTorrentPath)) {
		if (!QFile::remove(newTorrentPath)) {
			return false;
		}
	}

	if (!QFile::copy(filename, newTorrentPath)) {
		return false;
	}

	return true;
}

bool TorrentManager::removeTorrent(Torrent *torrent, bool deleteData)
{
	QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QFile savedTorrentFile(dataPath + "/resume/" + torrent->torrentInfo()->infoHash().toHex() + ".torrent");
	if (savedTorrentFile.exists()) {
		savedTorrentFile.remove();
	}
    _torrents.removeAll(torrent);
	if (deleteData) {
		for (QFile *file : torrent->files()) {
			if (file->exists()) {
				file->remove();
			}
		}
	}
	torrent->stop();
	emit torrentRemoved(torrent);
	torrent->deleteLater();
	return true;
}

const QList<Torrent *> &TorrentManager::torrents() const
{
    return _torrents;
}
