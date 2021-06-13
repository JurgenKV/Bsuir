#include "block.h"
#include "piece.h"
#include "peer.h"

Block::Block(Piece *piece, int begin, int size)
    : _piece(piece)
    , _begin(begin)
    , _size(size)
    , _isDownloaded(false)
{
    connect(this, &Block::downloaded, _piece, &Piece::updateState);
}

Block::~Block()
{
}

Piece *Block::piece()
{
    return _piece;
}

int Block::begin() const
{
    return _begin;
}

int Block::size() const
{
    return _size;
}

bool Block::isDownloaded()
{
    return _isDownloaded;
}

QList<Peer *> &Block::assignees()
{
    return _assignees;
}

bool Block::hasAssignees() const
{
    return !_assignees.isEmpty();
}

void Block::setDownloaded(bool isDownloaded)
{
    _isDownloaded = isDownloaded;
	if (isDownloaded) {
		emit downloaded(this);
	}
}

void Block::setData(const Peer *peer, const char *data)
{
	if (isDownloaded()) {
		return;
	}

    char *p = _piece->data() + _begin;
    for (int i = 0; i < _size; i++) {
		p[i] = data[i];
	}
	setDownloaded(true);
    QList<Peer *> assignees = _assignees;
	for (auto p : assignees) {
		if (p != peer) {
			p->sendCancel(this);
		}
		p->releaseBlock(this);
	}
}

void Block::addAssignee(Peer *peer)
{
    _assignees.push_back(peer);
}

void Block::removeAssignee(Peer *peer)
{
    for (int i = _assignees.size() - 1; i >= 0; i--) {
        if (_assignees[i] == peer) {
            _assignees.removeAt(i);
		}
	}
}

void Block::clearAssignees()
{
    _assignees.clear();
}
