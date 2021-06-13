#ifndef TORRENT_H
#define TORRENT_H

#include "resumeinfo.h"
#include "trackerclient.h"
#include <QHostAddress>
#include <QString>
#include <QList>
#include <QUrl>

class Peer;
class TorrentInfo;
class TrackerClient;
class FileController;
class TrafficMonitor;
class Piece;
class Block;
class QFile;

class Torrent : public QObject
{
	Q_OBJECT

public:
	enum State {
		New, Loading, Checking, Stopped, Started
	};

	Torrent();
	~Torrent();

	bool createNew(TorrentInfo *torrentInfo, const QString &downloadLocation);
	bool createFromResumeInfo(TorrentInfo *torrentInfo, ResumeInfo *resumeInfo);
	void loadFileDescriptors();

	Block *requestBlock(Peer *client, int size);

	bool savePiece(Piece *piece);

	QList<Peer *> &peers();
	QList<Piece *> &pieces();
	QList<QFile *> &files();
	TorrentInfo *torrentInfo();
	TrackerClient *trackerClient();
	TrafficMonitor *trafficMonitor();

	qint64 bytesDownloaded() const;
	qint64 bytesUploaded() const;
	qint64 totalBytesDownloaded() const;
	qint64 totalBytesUploaded() const;
	qint64 bytesAvailable() const;
	qint64 bytesLeft() const;

	int downloadedPieces();
	bool isDownloaded();
	bool isPaused() const;
	bool isStarted() const;
	int connectedPeersCount() const;
	int allPeersCount() const;

	State state() const;
	QString stateString() const;

	const QString &downloadLocation() const;

	float percentDownloaded();
	QVector<bool> bitfield() const;

	ResumeInfo getResumeInfo() const;

	QString errorString() const;

signals:
	void checkingStarted();
	void checked();
	void fullyDownloaded();

public slots:
	void onChecked();

	void onPieceDownloaded(Piece *piece);

	void onBlockUploaded(int bytes);

	void onFullyDownloaded();

	void onSuccessfullyAnnounced(TrackerClient::Event event);

	Peer *connectToPeer(QHostAddress address, int port);
	void addPeer(Peer *peer);
	void setPieceAvailable(Piece *piece, bool available);

	void start();
	void pause();
	void stop();
	void check();

private:
    State _state;
    QList<Peer *> _peers;
    QList<Piece *> _pieces;
    TorrentInfo *_torrentInfo;
    TrackerClient *_trackerClient;
    QList<QFile *> _files;
    FileController *_fileController;
    TrafficMonitor *_trafficMonitor;

    qint64 _bytesDownloadedOnStartup;
    qint64 _bytesUploadedOnStartup;
    qint64 _totalBytesDownloaded;
    qint64 _totalBytesUploaded;
    qint64 _bytesAvailable;

    int _downloadedPieces;
    bool _isDownloaded;
    bool _isPaused;
    bool _startAfterChecking;

    QString _downloadLocation;

    QString _errorString;
	void clearError();
	void setError(const QString &errorString);
};

#endif
