QT += core network gui widgets

CONFIG += c++11

TARGET = JTorrent

TEMPLATE = app

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp \
    global.cpp \
    core/bencodeparser.cpp \
    core/bencodevalue.cpp \
    core/torrentinfo.cpp \
    core/trackerclient.cpp \
    core/torrent.cpp \
    core/peer.cpp \
    core/piece.cpp \
    core/block.cpp \
    core/torrentmessage.cpp \
    core/torrentserver.cpp \
    core/resumeinfo.cpp \
    core/torrentmanager.cpp \
    core/torrentsettings.cpp \
    core/remote.cpp \
    core/filecontroller.cpp \
    core/localservicediscoveryclient.cpp \
    core/trafficmonitor.cpp \
    jtorrent.cpp \
    ui/mainwindow.cpp \
    ui/panel.cpp \
    ui/torrentslist.cpp \
    ui/torrentslistitem.cpp \
    ui/addtorrentdialog.cpp \
    ui/torrentitemdelegate.cpp \
    ui/settingswindow.cpp

HEADERS += \
    global.h \
    core/bencodeparser.h \
    core/bencodevalue.h \
    core/torrentinfo.h \
    core/trackerclient.h \
    core/torrent.h \
    core/peer.h \
    core/piece.h \
    core/block.h \
    core/torrentmessage.h \
    core/torrentserver.h \
    core/resumeinfo.h \
    core/torrentmanager.h \
    core/torrentsettings.h \
    core/remote.h \
    core/filecontroller.h \
    core/localservicediscoveryclient.h \
    core/trafficmonitor.h \
    jtorrent.h \
    ui/mainwindow.h \
    ui/panel.h \
    ui/torrentslist.h \
    ui/torrentslistitem.h \
    ui/addtorrentdialog.h \
    ui/torrentitemdelegate.h \
    ui/settingswindow.h

RESOURCES += \

# UNIX-specific configuration
unix:!macx: include(../unixconf.pri)
