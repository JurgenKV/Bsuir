#ifndef TORRENTINFO_H
#define TORRENTINFO_H

#include <QList>
#include <QString>
#include <QDateTime>

struct FileInfo {
	QList<QString> path;
	qint64 length;
};

class TorrentInfo
{
    QString _errorString;
	void clearError();
	void setError(QString errorString);

    QList<QByteArray> _announceUrlsList;

    qint64 _length;
    QByteArray _torrentName;
    qint64 _pieceLength;
    QList<QByteArray> _pieces;

    QDateTime *_creationDate;
    QString *_comment;
    QString *_createdBy;
    QString *_encoding;

    QList<FileInfo> _fileInfos;

    QByteArray _infoHash;

    QString _creationFileName;

    int _numberOfPieces;
public:
	QString errorString() const;
	bool loadFromTorrentFile(QString filename);

	const QList<QByteArray> &announceUrlsList() const;

	qint64 length() const;
	const QByteArray &torrentName() const;
	qint64 pieceLength() const;
	const QList<QByteArray> &pieces() const;
	const QByteArray &piece(int pieceIndex) const;

	const QDateTime *creationDate() const;
	const QString *comment() const;
	const QString *createdBy() const;
	const QString *encoding() const;

	const QList<FileInfo>& fileInfos() const;
	bool isSingleFile() const;

	const QByteArray &infoHash() const;

	const QString &creationFileName() const;

	int numberOfPieces() const;
	int bitfieldSize() const;

	TorrentInfo();
	~TorrentInfo();
};

#endif
