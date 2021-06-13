#include "torrentinfo.h"
#include "bencodeparser.h"
#include <QFile>
#include <QString>
#include <QCryptographicHash>
#include <QDebug>

void TorrentInfo::clearError()
{
    _errorString.clear();
}

void TorrentInfo::setError(QString errorString)
{
    _errorString = errorString;
}

QString TorrentInfo::errorString() const
{
    return _errorString;
}

TorrentInfo::TorrentInfo()
    : _length(0)
    , _pieceLength(0)
    , _creationDate(nullptr)
    , _comment(nullptr)
    , _createdBy(nullptr)
    , _encoding(nullptr)
    , _numberOfPieces(0)
{
}

TorrentInfo::~TorrentInfo()
{
    if (_creationDate != nullptr) delete _creationDate;
    if (_comment != nullptr) delete _comment;
    if (_createdBy != nullptr) delete _createdBy;
    if (_encoding != nullptr) delete _encoding;
}

bool TorrentInfo::loadFromTorrentFile(QString filename)
{
    _creationFileName = filename;
	BencodeParser bencodeParser;

	if (!bencodeParser.readFile(filename)) {
        setError("Ошибка чтения файла " + filename + ": " + bencodeParser.errorString());
		return false;
	}

	if (!bencodeParser.parse()) {
        setError("Ошибка парсинга файла " + filename + ": " + bencodeParser.errorString());
		return false;
	}

	try {
		BencodeException ex("TorrentInfo::loadFromTorrentFile(): ");

		QList<BencodeValue *> mainList = bencodeParser.list();

		if (mainList.isEmpty()) {
            throw ex << "Торрент файл пуст";
		} else if (mainList.size() > 1) {
			throw ex << "Main list size is " << mainList.size() << ". Expected 1";
		}

		BencodeDictionary *mainDict = mainList.first()->toBencodeDictionary();

		BencodeDictionary *infoDict = mainDict->value("info")->toBencodeDictionary();

		try {
			QList<QByteArray> announceUrlsList;
			QList<BencodeValue*> announceList = mainDict->value("announce-list")->toList();
			for (BencodeValue *announceListValue : announceList) {
				QList<BencodeValue *> announceSubList = announceListValue->toList();
				for (BencodeValue *announceUrl : announceSubList) {
					announceUrlsList.push_back(announceUrl->toByteArray());
				}
			}
            _announceUrlsList = announceUrlsList;
		} catch (BencodeException &ex) {
            _announceUrlsList.clear();
			try {
				QByteArray url = mainDict->value("announce")->toByteArray();
                _announceUrlsList.push_back(url);
			} catch(BencodeException &ex) {
                _announceUrlsList.clear();
			}
		}

        _torrentName = infoDict->value("name")->toByteArray();

        _pieceLength = infoDict->value("piece length")->toInt();

		QByteArray pieceData = infoDict->value("pieces")->toByteArray();
		if (pieceData.size() % 20 != 0) {
			throw ex << "Piece data length is not a multiple of 20";
		}
		for (int i = 0; i < pieceData.size();) {
			QByteArray piece;
			for (int j = 0; j < 20; j++) {
				piece.append(pieceData[i++]);
			}
            _pieces.append(piece);
		}

		if (infoDict->keyExists("length")) {
            _length = infoDict->value("length")->toInt();
			FileInfo fileInfo;
            fileInfo.length = _length;
            fileInfo.path = QList<QString>({_torrentName});
            _fileInfos.push_back(fileInfo);
		} else {
            _length = 0;
			QList<BencodeValue *> filesList = infoDict->value("files")->toList();
			for (BencodeValue *file : filesList) {
				BencodeDictionary *fileDict = file->toBencodeDictionary();
				FileInfo fileInfo;
				fileInfo.length = fileDict->value("length")->toInt();
				QList<BencodeValue *> pathList = fileDict->value("path")->toList();
                fileInfo.path = QList<QString>({_torrentName});
				for (auto path : pathList) {
					fileInfo.path.push_back(path->toByteArray());
				}
                _length += fileInfo.length;
                _fileInfos.push_back(fileInfo);
			}
		}
		try {
            _creationDate = new QDateTime;
			qint64 creation = mainDict->value("creation date")->toInt();
            *_creationDate = QDateTime::fromMSecsSinceEpoch(1000*creation);
		} catch (BencodeException &ex) {
            delete _creationDate;
            _creationDate = nullptr;
		}

		try {
            _comment = new QString;
            *_comment = mainDict->value("comment")->toByteArray();
		} catch (BencodeException &ex) {
            delete _comment;
            _comment = nullptr;
		}

		try {
            _createdBy = new QString;
            *_createdBy = mainDict->value("created by")->toByteArray();
		} catch (BencodeException &ex) {
            delete _createdBy;
            _createdBy = nullptr;
		}

		try {
            _encoding = new QString;
            *_encoding = mainDict->value("encoding")->toByteArray();
		} catch (BencodeException &ex) {
            delete _encoding;
            _encoding = new QString("UTF-8");
		}
        _infoHash = QCryptographicHash::hash(infoDict->getRawBencodeData(), QCryptographicHash::Sha1);
        _numberOfPieces = _length / _pieceLength;
        if (_length % _pieceLength != 0) {
            _numberOfPieces++;
		}
	} catch (BencodeException &ex) {
		setError(ex.what());
		return false;
	}
	return true;
}


const QList<QByteArray> &TorrentInfo::announceUrlsList() const
{
    return _announceUrlsList;
}

qint64 TorrentInfo::length() const
{
    return _length;
}

const QByteArray &TorrentInfo::torrentName() const
{
    return _torrentName;
}

qint64 TorrentInfo::pieceLength() const
{
    return _pieceLength;
}

const QList<QByteArray> &TorrentInfo::pieces() const
{
    return _pieces;
}

const QByteArray &TorrentInfo::piece(int pieceIndex) const
{
    return _pieces[pieceIndex];
}

const QList<FileInfo> &TorrentInfo::fileInfos() const
{
    return _fileInfos;
}

bool TorrentInfo::isSingleFile() const
{
    return (_fileInfos.size() == 1);
}

const QByteArray &TorrentInfo::infoHash() const
{
    return _infoHash;
}

const QString &TorrentInfo::creationFileName() const
{
    return _creationFileName;
}

int TorrentInfo::numberOfPieces() const
{
    return _numberOfPieces;
}

int TorrentInfo::bitfieldSize() const
{
    int size = _numberOfPieces / 8;
    if (_numberOfPieces % 8 != 0) {
		size++;
	}
	return size;
}


const QDateTime *TorrentInfo::creationDate() const
{
    return _creationDate;
}

const QString *TorrentInfo::comment() const
{
    return _comment;
}

const QString *TorrentInfo::createdBy() const
{
    return _createdBy;
}

const QString *TorrentInfo::encoding() const
{
    return _encoding;
}
