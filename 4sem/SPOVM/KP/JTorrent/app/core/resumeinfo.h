#ifndef RESUMEINFO_H
#define RESUMEINFO_H

#include <QtGlobal>
#include <QVector>
#include <QByteArray>

class TorrentInfo;
class BencodeDictionary;

class ResumeInfo
{
public:
	ResumeInfo(TorrentInfo *torrentInfo);

	bool loadFromBencode(BencodeDictionary *dict);

	void addToBencode(BencodeDictionary *mainResumeDictionary) const;

	TorrentInfo *torrentInfo() const;
	const QString &downloadLocation() const;
	qint64 totalBytesDownloaded() const;
	qint64 totalBytesUploaded() const;
	bool paused() const;
	const QVector<bool> &aquiredPieces() const;
	QByteArray aquiredPiecesArray() const;

	void setDownloadLocation(const QString &downloadLocation);
	void setTotalBytesDownloaded(qint64 totalBytesDownloaded);
	void setTotalBytesUploaded(qint64 totalBytesUploaded);
	void setPaused(bool paused);
	void setAquiredPieces(const QVector<bool> &aquiredPieces);

private:
    TorrentInfo *_torrentInfo;
    QString _downloadLocation;
    qint64 _totalBytesDownloaded;
    qint64 _totalBytesUploaded;
    bool _paused;
    QVector<bool> _aquiredPieces;

	QVector<bool> toBitArray(const QByteArray &data);
};

#endif
