#include "core/resumeinfo.h"
#include "core/bencodevalue.h"
#include "core/torrentinfo.h"
#include <QDebug>

ResumeInfo::ResumeInfo(TorrentInfo *torrentInfo)
    : _torrentInfo(torrentInfo)
    , _totalBytesDownloaded(0)
    , _totalBytesUploaded(0)
    , _paused(false)
{
}


bool ResumeInfo::loadFromBencode(BencodeDictionary *dict)
{
	try {
        _downloadLocation = dict->value("downloadLocation")->toByteArray();
        _totalBytesDownloaded = dict->value("totalBytesDownloaded")->toInt();
        _totalBytesUploaded = dict->value("totalBytesUploaded")->toInt();
        _paused = dict->value("paused")->toInt() ? true : false;
        _aquiredPieces = toBitArray(dict->value("aquiredPieces")->toByteArray());

	} catch (BencodeException &ex) {
		qDebug() << "Failed to load resume info:" << ex.what();
		return false;
	}
	return true;
}

void ResumeInfo::addToBencode(BencodeDictionary *mainResumeDictionary) const
{
	BencodeDictionary *dict = new BencodeDictionary;
    dict->add("downloadLocation", new BencodeString(_downloadLocation.toUtf8()));
    dict->add("totalBytesDownloaded", new BencodeInteger(_totalBytesDownloaded));
    dict->add("totalBytesUploaded", new BencodeInteger(_totalBytesUploaded));
    dict->add("paused", new BencodeInteger(_paused));
	dict->add("aquiredPieces", new BencodeString(aquiredPiecesArray()));

    mainResumeDictionary->add(_torrentInfo->infoHash(), dict);
}


QByteArray ResumeInfo::aquiredPiecesArray() const
{
	QByteArray ret;
    for (int i = 0; i < _aquiredPieces.size()/8; i++) {
		unsigned char byte = 0;
		for (int j = 0; j < 8; j++) {
            if (_aquiredPieces[i * 8 + j]) {
				byte |= (1 << (7-j));
			}
		}
		ret.push_back(byte);
	}
    if ((_aquiredPieces.size() % 8) != 0) {
		unsigned char byte = 0;
        int bits = _aquiredPieces.size() % 8;
        int i = _aquiredPieces.size() - bits;
		for (int j = 0; j < bits; j++) {
            if (_aquiredPieces[i+j]) {
				byte |= (1 << (7 - j));
			}
		}
		ret.push_back(byte);
	}
	return ret;
}

QVector<bool> ResumeInfo::toBitArray(const QByteArray &data)
{
	QVector<bool> ret;
	for (unsigned char byte : data) {
		for (int i = 7; i >= 0; i--) {
			ret.push_back((byte & (1 << i)) ? true : false);
		}
	}
    int trailingBits = 8 - (_torrentInfo->numberOfPieces() % 8);
	if (trailingBits > 0 && trailingBits < 8) {
		ret.remove(ret.size() - trailingBits, trailingBits);
	}
	return ret;
}


TorrentInfo *ResumeInfo::torrentInfo() const
{
    return _torrentInfo;
}

const QString &ResumeInfo::downloadLocation() const
{
    return _downloadLocation;
}

qint64 ResumeInfo::totalBytesDownloaded() const
{
    return _totalBytesDownloaded;
}

qint64 ResumeInfo::totalBytesUploaded() const
{
    return _totalBytesUploaded;
}

bool ResumeInfo::paused() const
{
    return _paused;
}

const QVector<bool> &ResumeInfo::aquiredPieces() const
{
    return _aquiredPieces;
}


void ResumeInfo::setDownloadLocation(const QString &downloadLocation)
{
    _downloadLocation = downloadLocation;
}

void ResumeInfo::setTotalBytesDownloaded(qint64 totalBytesDownloaded)
{
    _totalBytesDownloaded = totalBytesDownloaded;
}

void ResumeInfo::setTotalBytesUploaded(qint64 totalBytesUploaded)
{
    _totalBytesUploaded = totalBytesUploaded;
}

void ResumeInfo::setPaused(bool paused)
{
    _paused = paused;
}

void ResumeInfo::setAquiredPieces(const QVector<bool> &aquiredPieces)
{
    _aquiredPieces = aquiredPieces;
}
