#ifndef PIECE_H
#define PIECE_H

#include <QObject>
#include <QList>
#include <QByteArray>

class Torrent;
class Block;

class Piece : public QObject
{
	Q_OBJECT

private:
    Torrent *_torrent;
    int _pieceNumber;
    int _size;
    bool _isDownloaded;
    char *_pieceData;
    QList<Block *> _blocks;

	void addBlock(Block *block);
	bool checkIfFullyDownloaded();

public:
	Piece(Torrent *torrent, int pieceNumber, int size);
	~Piece();

	bool isDownloaded() const;
	int pieceNumber() const;
	char *data() const;
	int size() const;

	bool getBlockData(int begin, int size, QByteArray &blockData);
	bool getPieceData(QByteArray &pieceData);
	Block *getBlock(int begin, int size) const;
	Block *requestBlock(int size);

signals:
	void availabilityChanged(Piece *piece, bool isDownloaded);

public slots:
	void updateState();
	void deleteBlock(Block *block);
	void unloadFromMemory();
	void setDownloaded(bool isDownloaded);
};

#endif
