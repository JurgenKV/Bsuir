#ifndef TORRENTSETTINGS_H
#define TORRENTSETTINGS_H

#include <QString>

class TorrentSettings
{
public:
	TorrentSettings();

	void setDownloadLocation(const QString &downloadLocation);
	void setStartImmediately(bool startImmediately);
	void setSkipHashCheck(bool skipHashCheck);

	const QString &downloadLocation() const;
	bool startImmediately() const;
	bool skipHashCheck() const;

private:
    QString _downloadLocation;
    bool _startImmediately;
    bool _skipHashCheck;
};

#endif
