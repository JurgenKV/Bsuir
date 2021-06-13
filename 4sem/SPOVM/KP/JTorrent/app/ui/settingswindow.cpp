#include "settingswindow.h"
#include "jtorrent.h"
#include "core/torrentserver.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QSettings>
#include <QMessageBox>
#include <QGuiApplication>

SettingsWindow::SettingsWindow(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	setLayout(mainLayout);

    _serverStartPort = new QLineEdit;
    _serverEndPort = new QLineEdit;
    _serverStartPort->setValidator(new QIntValidator(0, 49151));
    _serverEndPort->setValidator(new QIntValidator(0, 49151));
    _serverStartPort->setMaximumWidth(200);
    _serverEndPort->setMaximumWidth(200);

	QHBoxLayout *serverPortLayout = new QHBoxLayout;
    serverPortLayout->addWidget(new QLabel(tr("Диапазон портов от : ")));
    serverPortLayout->addWidget(_serverStartPort);
    serverPortLayout->addWidget(new QLabel(tr(" до ")));
    serverPortLayout->addWidget(_serverEndPort);
	serverPortLayout->addStretch();

	mainLayout->addLayout(serverPortLayout);

    QPushButton *applyButton = new QPushButton(tr("Подтвердить"));
    QPushButton *cancelButton = new QPushButton(tr("Отмена"));
	applyButton->setMaximumWidth(200);
	cancelButton->setMaximumWidth(200);

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(applyButton);
	buttonsLayout->addWidget(cancelButton);
	buttonsLayout->addStretch();

	mainLayout->addLayout(buttonsLayout);

	mainLayout->addStretch();

	connect(applyButton, &QPushButton::clicked, this, &SettingsWindow::apply);
	connect(cancelButton, &QPushButton::clicked, this, &SettingsWindow::reset);

	reset();
}

void SettingsWindow::apply()
{
	bool ok;

    quint16 serverStartPort = _serverStartPort->text().toInt(&ok);
	if (!ok) {
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(), tr("Введите правильный начальный порт"));
		return;
	}

    quint16 serverEndPort = _serverEndPort->text().toInt(&ok);
	if (!ok) {
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(), tr("Введите правильный конечный порт"));
		return;
	}

	QSettings settings;
	settings.setValue("ServerStartPort", serverStartPort);
	settings.setValue("ServerEndPort", serverEndPort);

    JTorrent::instance()->server()->startServer();
}

void SettingsWindow::reset()
{
	QSettings settings;
    _serverStartPort->setText(settings.value("ServerStartPort").toString());
    _serverEndPort->setText(settings.value("ServerEndPort").toString());
}
