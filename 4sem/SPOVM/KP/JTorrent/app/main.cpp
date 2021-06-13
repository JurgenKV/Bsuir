#include "jtorrent.h"
#include "core/remote.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    app.setOrganizationName("JTorrent");
    app.setApplicationName("JTorrent");

	Remote remote;
	if (!remote.start()) {
        qDebug() << "Запущено";
		return 0;
	}

    JTorrent JTorrent;

	app.exec();
    JTorrent.shutDown();
	return 0;
}
