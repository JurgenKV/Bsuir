#include "filecontroller.h"
#include "torrent.h"
#include "torrentinfo.h"
#include "piece.h"
#include <QCryptographicHash>
#include <QThread>

FileController::FileController(Torrent *torrent)
    : _torrent(torrent)
    , _workerThread(new QThread)
{
	FileControllerWorker *worker = new FileControllerWorker(torrent);
    worker->moveToThread(_workerThread);
    connect(_workerThread, &QThread::finished, worker, &FileControllerWorker::deleteLater);

    _workerThread->start();

	connect(this, &FileController::checkTorrent, worker, &FileControllerWorker::checkTorrent);
	connect(worker, &FileControllerWorker::torrentChecked, this, &FileController::torrentChecked);
    connect(worker, &FileControllerWorker::pieceAvailable, _torrent, &Torrent::setPieceAvailable);
}

FileController::~FileController()
{
    _workerThread->quit();
    _workerThread->wait();
    delete _workerThread;
}


FileControllerWorker::FileControllerWorker(Torrent *torrent)
    : _torrent(torrent)
{
}

void FileControllerWorker::checkTorrent()
{
    TorrentInfo *info = _torrent->torrentInfo();
    QList<Piece *> &pieces = _torrent->pieces();
	for (Piece *piece : pieces) {
		emit pieceAvailable(piece, false);
	}
	for (Piece *piece : pieces) {
		QByteArray pieceData, pieceHash;
		if (!piece->getPieceData(pieceData)) {
			continue;
		}
		pieceHash = QCryptographicHash::hash(pieceData, QCryptographicHash::Sha1);
		bool pieceIsValid = (pieceHash == info->piece(piece->pieceNumber()));
		if (pieceIsValid) {
			emit pieceAvailable(piece, true);
		}
	}
	emit torrentChecked();
}
