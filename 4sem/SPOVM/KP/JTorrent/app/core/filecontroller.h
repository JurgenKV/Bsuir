#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QObject>

class QThread;
class Torrent;
class Piece;

class FileControllerWorker : public QObject
{
	Q_OBJECT

public:
	FileControllerWorker(Torrent *torrent);

public slots:
	void checkTorrent();

signals:
	void torrentChecked();
	void pieceAvailable(Piece* piece, bool available);

private:
    Torrent *_torrent;
};

class FileController : public QObject
{
	Q_OBJECT

public:
	FileController(Torrent *torrent);
	~FileController();

signals:
	void checkTorrent();
	void torrentChecked();

private:
    Torrent *_torrent;
    QThread *_workerThread;
};

#endif
