#include "bencodeparser.h"
#include "jtorrent.h"
#include "torrent.h"
#include "peer.h"
#include "torrentserver.h"
#include "trackerclient.h"
#include "global.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUrl>
#include <QDebug>

TrackerClient::TrackerClient(Torrent *torrent)
    : _torrent(torrent)
    , _reply(nullptr)
    , _reannounceInterval(-1)
    , _currentAnnounceListIndex(0)
    , _hasAnnouncedStarted(false)
    , _numberOfAnnounces(0)
    , _lastEvent(None)
{
    connect(&_reannounceTimer, SIGNAL(timeout()), this, SLOT(reannounce()));
}

TrackerClient::~TrackerClient()
{
}


void TrackerClient::reannounce()
{
	announce(Event::None);
}

void TrackerClient::announce(Event event)
{
    if(_torrent->torrentInfo()->announceUrlsList().isEmpty()) {
		return;
	}
    _lastEvent = event;
	QUrl url;
	url.setUrl(currentAnnounceUrl());

    qint64 bytesDownloaded = _torrent->bytesDownloaded();
    qint64 bytesUploaded = _torrent->bytesUploaded();
    qint64 bytesLeft = _torrent->bytesLeft();
    int port = JTorrent::instance()->server()->port();

	QString bytesDownloadedString = QString::number(bytesDownloaded);
	QString bytesUploadedString = QString::number(bytesUploaded);
	QString bytesLeftString = QString::number(bytesLeft);
	QString portString = QString::number(port);

	QUrlQuery query(url);
    auto hash = percentEncode(_torrent->torrentInfo()->infoHash());
	query.addQueryItem("info_hash", hash);
    query.addQueryItem("peer_id", percentEncode(JTorrent::instance()->peerId()));
	query.addQueryItem("port", portString);
	query.addQueryItem("uploaded", bytesUploadedString);
	query.addQueryItem("downloaded", bytesDownloadedString);
	query.addQueryItem("left", bytesLeftString);
	query.addQueryItem("compact", "1");
	if (event == Event::Started) {
		query.addQueryItem("event", "started");
	} else if (event == Event::Stopped) {
		query.addQueryItem("event", "stopped");
		query.addQueryItem("numwant", "0");
	} else if (event == Event::Completed) {
		query.addQueryItem("event", "completed");
	}

	url.setQuery(query);
	qDebug() << "Announce" << url.toString();
	if (event == Stopped) {
        _hasAnnouncedStarted = false;
	}

	QNetworkRequest request(url);
    _reply = _accessManager.get(request);
    connect(_reply, &QNetworkReply::finished, this, &TrackerClient::httpFinished);
}

void TrackerClient::httpFinished()
{
    _reply->disconnect();
    _reply->deleteLater();

    if (_reply->error()) {
        qDebug() << "Error in httpFinished():" << _reply->errorString();
		announceFailed();
		return;
	}

    QVariant statusCodeVariant = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (statusCodeVariant.isValid()) {
		int statusCode = statusCodeVariant.toInt();
		if (statusCode != 200) {
			if (statusCode >= 300 && statusCode < 400) {
                QUrl redirectUrl = _reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
				if (redirectUrl.isEmpty()) {
					qDebug() << "Redirect URL is empty";
				} else {
					qDebug() << "Redirecting to" << redirectUrl;
                    _reply = _accessManager.get(QNetworkRequest(redirectUrl));
                    connect(_reply, &QNetworkReply::finished, this, &TrackerClient::httpFinished);
					return;
				}
			} else {
                QString reason = _reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
				qDebug() << "Error: Status code" << statusCode << ":" << reason;
			}
			announceFailed();
			return;
		}
	}

    QByteArray announceReply = _reply->readAll();
	BencodeParser bencodeParser;

	try {
		BencodeException ex("TrackerClient::httpFinished(): ");

		if (!bencodeParser.parse(announceReply)) {
			throw ex << "Parse failed" << endl << bencodeParser.errorString();
		}

		QList<BencodeValue *> responseMainList = bencodeParser.list();
		if (responseMainList.isEmpty()) {
			throw ex << "Tracker sent an empty response";
		} else if (responseMainList.size() > 1) {
			throw ex << "Tracker response main list has a size of " << responseMainList.size() << ". Expected 1";
		}

		BencodeDictionary* mainDict = responseMainList.first()->toBencodeDictionary();

		if (mainDict->keyExists("failure reason")) {
			qDebug() << "Error: Failure reason: " << mainDict->value("failure reason")->toByteArray();
			announceFailed();
			return;
		}

        _reannounceInterval = mainDict->value("interval")->toInt();
        _reannounceTimer.setInterval(_reannounceInterval*1000);
        _reannounceTimer.start();

		BencodeValue *peers = mainDict->value("peers");
		if (peers->isString()) {

			QByteArray peersData = peers->toByteArray();
			if (peersData.size() % 6 != 0) {
				throw ex << "Peers string length is " << peersData.size() << ". Expected a multiple of 6";
			}
			int numberOfPeers = peersData.size() / 6;
			for (int i = 0, counter = 0; i < numberOfPeers; i++)  {
				quint32 peerAddr = 0;
				peerAddr |= (quint8)peersData[counter++] << 24;
				peerAddr |= (quint8)peersData[counter++] << 16;
				peerAddr |= (quint8)peersData[counter++] << 8;
				peerAddr |= (quint8)peersData[counter++];

				int peerPort = 0;
				peerPort += (unsigned char)peersData[counter++];
				peerPort *= 256;
				peerPort += (unsigned char)peersData[counter++];
                _torrent->connectToPeer(QHostAddress(peerAddr), peerPort);
			}
		} else {
			QList<BencodeValue*> peersDictList = peers->toList();
			for (BencodeValue* peerDictValue : peersDictList) {
				BencodeDictionary* peerDict = peerDictValue->toBencodeDictionary();
				QByteArray ip = peerDict->value("ip")->toByteArray();
				int port = peerDict->value("port")->toInt();
                _torrent->connectToPeer(QHostAddress(QString(ip)), port);
			}
		}
	} catch (BencodeException &ex) {
		qDebug() << "Failed to parse tracker response:" << ex.what() << endl
				 << ">>>>>>>>>>>>>>>>>>>>" << endl
				 << announceReply << endl
				 << "<<<<<<<<<<<<<<<<<<<<";
		announceFailed();
		return;
	}
	announceSucceeded();
}

bool TrackerClient::nextAnnounceUrl()
{
    const QList<QByteArray> &list = _torrent->torrentInfo()->announceUrlsList();
    _currentAnnounceListIndex++;
    if (_currentAnnounceListIndex == list.size()) {
        _currentAnnounceListIndex = 0;
		return false;
	}
	return true;
}

const QByteArray &TrackerClient::currentAnnounceUrl() const
{
    auto &list = _torrent->torrentInfo()->announceUrlsList();
    return list[_currentAnnounceListIndex];
}

void TrackerClient::resetCurrentAnnounceUrl()
{
    _currentAnnounceListIndex = 0;
}

void TrackerClient::announceFailed()
{
	if (nextAnnounceUrl()) {
        announce(_lastEvent);
	} else {
		resetCurrentAnnounceUrl();
		qDebug() << "No more backup URLs";
	}
}

void TrackerClient::announceSucceeded()
{
	resetCurrentAnnounceUrl();
    _numberOfAnnounces++;
    if (_lastEvent == Started) {
        _hasAnnouncedStarted = true;
	}
    _torrent->onSuccessfullyAnnounced(_lastEvent);
}

int TrackerClient::numberOfAnnounces() const
{
    return _numberOfAnnounces;
}

bool TrackerClient::hasAnnouncedStarted() const
{
    return _hasAnnouncedStarted;
}
