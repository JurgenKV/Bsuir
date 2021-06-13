#ifndef PANEL_H
#define PANEL_H

#include <QToolBar>
#include <QIcon>
#include <QList>

class QToolButton;

class Panel : public QToolBar
{
	Q_OBJECT

public:
	enum Section {
        All, Settings
	};

	Panel(QWidget *parent = nullptr);
	~Panel();
	Section getCurrentSection();

public slots:
	void openAll();
	void openSettings();

signals:
	void showAll();
	void showSettings();
	void hideSettings();

private:
	void resetButtons();

    QList<QToolButton *> _toolButtons;

    QToolButton *_all;
    QToolButton *_settings;

};

#endif
