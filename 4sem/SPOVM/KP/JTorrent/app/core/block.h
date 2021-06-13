#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QByteArray>
#include <QList>

class Piece;
class Peer;

class Block : public QObject
{
	Q_OBJECT

private:
    Piece *_piece;
    int _begin;
    int _size;
    bool _isDownloaded;

    QList<Peer *> _assignees;

public:
	Block(Piece *piece, int begin, int size);
	~Block();
	Piece *piece();
	int begin() const;
	int size() const;
	bool isDownloaded();
	QList<Peer *> &assignees();
	bool hasAssignees() const;

signals:
	void downloaded(Block *block);

public slots:
	void setDownloaded(bool isDownloaded);
	void setData(const Peer *peer, const char *data);
	void addAssignee(Peer *peer);
	void removeAssignee(Peer *peer);
	void clearAssignees();
};

#endif
