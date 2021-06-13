#include "torrentsettings.h"

TorrentSettings::TorrentSettings()
{
}


void TorrentSettings::setDownloadLocation(const QString &downloadLocation)
{
    _downloadLocation = downloadLocation;
}

void TorrentSettings::setStartImmediately(bool startImmediately)
{
    _startImmediately = startImmediately;
}

void TorrentSettings::setSkipHashCheck(bool skipHashCheck)
{
    _skipHashCheck = skipHashCheck;
}


const QString &TorrentSettings::downloadLocation() const
{
    return _downloadLocation;
}

bool TorrentSettings::startImmediately() const
{
    return _startImmediately;
}

bool TorrentSettings::skipHashCheck() const
{
    return _skipHashCheck;
}
