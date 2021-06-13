#include "piece.h"
#include "block.h"
#include "torrent.h"
#include "torrentinfo.h"
#include "peer.h"
#include "trackerclient.h"
#include <QCryptographicHash>
#include <QTcpSocket>
#include <QFile>
#include <QDebug>

Piece::Piece(Torrent *torrent, int pieceNumber, int size)
    : _torrent(torrent)
    , _pieceNumber(pieceNumber)
    , _size(size)
    , _isDownloaded(false)
    , _pieceData(nullptr)
{
}

Piece::~Piece()
{
    for (auto b : _blocks) {
		delete b;
	}
    if (_pieceData != nullptr) {
        delete[] _pieceData;
	}
}

bool Piece::isDownloaded() const
{
    return _isDownloaded;
}

int Piece::pieceNumber() const
{
    return _pieceNumber;
}

char *Piece::data() const
{
    return _pieceData;
}

int Piece::size() const
{
    return _size;
}

void Piece::addBlock(Block *block)
{
	int insertPos = 0;
    for (int i = 0; i < _blocks.size(); i++) {
        if (block->begin() < _blocks[i]->begin()) {
			break;
		}
		insertPos++;
	}
    _blocks.insert(insertPos, block);
}

void Piece::deleteBlock(Block *block)
{
	int blockNumber = -1;
    for (int i = 0; i < _blocks.size(); i++) {
        if (_blocks[i] == block) {
			blockNumber = i;
			break;
		}
	}
	if (blockNumber != -1) {
        _blocks[blockNumber]->deleteLater();
        _blocks.removeAt(blockNumber);
	}
}

bool Piece::checkIfFullyDownloaded()
{
    if (_isDownloaded) {
		return true;
	}
    Q_ASSERT_X(_pieceData != nullptr, "Piece::checkIfFullyDownloaded()", "Piece not loaded");
	int pos = 0;
    for (auto b : _blocks) {
		if (b->begin() == pos && b->isDownloaded()) {
			pos += b->size();
		} else {
			return false;
		}
	}
    return (_size == pos);
}

void Piece::updateState()
{
	if (checkIfFullyDownloaded()) {
        Q_ASSERT_X(_pieceData != nullptr, "Piece::updateState()", "Piece not loaded");
		QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(_pieceData, _size);
		QByteArray actualHash = hash.result();
        if (actualHash != _torrent->torrentInfo()->piece(_pieceNumber)) {
            for (auto b : _blocks) {
				b->deleteLater();
			}
            _blocks.clear();
			setDownloaded(false);
            qDebug() << "Piece" << _pieceNumber << "failed SHA1 validation";
		} else {
            _torrent->savePiece(this);
			setDownloaded(true);
			unloadFromMemory();
            _torrent->onPieceDownloaded(this);
		}
	}
}

Block *Piece::requestBlock(int size)
{
	int tmp = 0;
	int s = size;
    if (_isDownloaded) {
		return nullptr;
	}
    if (_pieceData == nullptr) {
        _pieceData = new char[_size];
	}
	Block *block = nullptr;

    for (auto b : _blocks) {
		if (!b->isDownloaded() && !b->hasAssignees()) {
			return b;
		}
		if (tmp < b->begin()) {
			s = b->begin() - tmp;
			if (s > size) {
				s = size;
			}
			block = new Block(this, tmp, s);
			break;
		} else {
			tmp = b->begin() + b->size();
		}
	}
	if (block == nullptr) {
        int pieceSize = _size;
		if (pieceSize - tmp > size) {
			block = new Block(this, tmp, size);
		} else if (pieceSize - tmp > 0) {
			block = new Block(this, tmp, pieceSize - tmp);
		}
	}
	if (block != nullptr) {
		addBlock(block);
	}

	return block;
}

void Piece::unloadFromMemory()
{
    Q_ASSERT_X(_pieceData != nullptr, "Piece::unloadFromMemory()", "Piece is not loaded");
	Q_ASSERT_X(checkIfFullyDownloaded(), "Piece::unloadFromMemory()", "Piece is not fully downloaded");
    delete[] _pieceData;
    _pieceData = nullptr;
}

void Piece::setDownloaded(bool isDownloaded)
{
    _isDownloaded = isDownloaded;
    for (Block* block : _blocks) {
		block->deleteLater();
	}
    _blocks.clear();
    emit availabilityChanged(this, _isDownloaded);
}

bool Piece::getBlockData(int begin, int size, QByteArray &blockData)
{
	blockData.clear();
    if (_pieceData && _isDownloaded) {
        blockData.append(_pieceData + begin, size);
		return true;
	}

    const QList<FileInfo> &fileInfos = _torrent->torrentInfo()->fileInfos();
    QList<QFile *> &files = _torrent->files();

    qint64 blockBegin = _torrent->torrentInfo()->pieceLength();
    blockBegin *= _pieceNumber;
	blockBegin += begin;
	qint64 blockEnd = blockBegin + size;

	qint64 fileBegin = 0;
	for (int i = 0; i < fileInfos.size(); i++) {
		const FileInfo &fileInfo = fileInfos[i];
		QFile *file = files[i];

		qint64 fileEnd = fileBegin + fileInfo.length;

		if (fileEnd > blockBegin && fileBegin < blockEnd) {
			qint64 seek = 0;
			if (blockBegin - fileBegin > 0) {
				seek = blockBegin - fileBegin;
			}

			qint64 bytesToRead = qMin(blockEnd, fileEnd) - qMax(blockBegin, fileBegin);

			if (!file->open(QFile::ReadOnly)) {
				qDebug() << "Failed to open file" << file->fileName() << ":" << file->errorString();
				return false;
			}

			if (seek) {
				if (!file->seek(seek)) {
					qDebug() << "Failed to seek" << seek << "bytes in file" << file->fileName() << ":" << file->errorString();
					file->close();
					return false;
				}
			}

			blockData.append(file->read(bytesToRead));

			file->close();

			if (fileEnd >= blockEnd) {
				return true;
			}
		}

		fileBegin += fileInfo.length;
	}
	return true;
}

bool Piece::getPieceData(QByteArray &pieceData)
{
    return getBlockData(0, _size, pieceData);
}

Block *Piece::getBlock(int begin, int size) const
{
    for (Block *block : _blocks) {
		if (block->begin() == begin && block->size() == size) {
			return block;
		}
	}
	return nullptr;
}
