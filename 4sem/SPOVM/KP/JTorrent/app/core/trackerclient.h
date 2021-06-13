#ifndef TRACKERCLIENT_H
#define TRACKERCLIENT_H

#include "torrentinfo.h"
#include <QNetworkAccessManager>
#include <QTimer>

class BencodeParser;
class Torrent;


class TrackerClient : public QObject
{
public:
	enum Event {
        Started,
        Stopped,
        Completed,
        None
	};


	TrackerClient(Torrent *torrent);
	~TrackerClient();

	void announce(Event event);

	int numberOfAnnounces() const;

	bool hasAnnouncedStarted() const;

public slots:
	void httpFinished();

	void reannounce();

private:
	Q_OBJECT

    Torrent *_torrent;
    QNetworkAccessManager _accessManager;
    QNetworkReply *_reply;

    int _reannounceInterval;
    QTimer _reannounceTimer;

    int _currentAnnounceListIndex;

	bool nextAnnounceUrl();

    bool _hasAnnouncedStarted;

	const QByteArray &currentAnnounceUrl() const;

	void resetCurrentAnnounceUrl();

	void announceFailed();

	void announceSucceeded();

    int _numberOfAnnounces;

    Event _lastEvent;
};

#endif
