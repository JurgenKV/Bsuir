#include "panel.h"
#include <QToolButton>

#define ICON_SIZE QSize(100, 40)

Panel::Panel(QWidget *parent)
    : QToolBar("Рабочая панель", parent)

    , _all(new QToolButton)
    , _settings(new QToolButton)
{
    _toolButtons.append(_all);
    _toolButtons.append(_settings);
    _all->setFixedSize(100,40);
    _all->setText("Список \nТоррентов");
    _settings->setFixedSize(100,40);
    _settings->setText("Настройки");

    for (auto button : _toolButtons) {
		button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
		button->setCheckable(true);
		button->setAutoExclusive(true);
		addWidget(button);
	}

	setIconSize(ICON_SIZE);

	setFloatable(false);

    connect(_all, SIGNAL(clicked()), this, SLOT(openAll()));
    connect(_settings, SIGNAL(clicked()), this, SLOT(openSettings()));
}

Panel::~Panel()
{
    for (auto button : _toolButtons) {
		delete button;
	}
}


Panel::Section Panel::getCurrentSection()
{
    if (_all->isChecked()) {
		return Section::All;
    } else if (_settings->isChecked()) {
		return Section::Settings;
	}
	Q_ASSERT(false);
	return Section::All;
}


void Panel::openAll()
{
    _all->setChecked(true);
	emit showAll();
	emit hideSettings();
}


void Panel::openSettings()
{
    _settings->setChecked(true);
	emit showSettings();
}
