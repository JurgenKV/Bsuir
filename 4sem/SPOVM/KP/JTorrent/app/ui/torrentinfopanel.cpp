/* qTorrent - An open-source, cross-platform BitTorrent client
 * Copyright (C) 2017 Petko Georgiev
 *
 * torrentinfopanel.cpp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "qtorrent.h"
#include "mainwindow.h"
#include "torrentslist.h"
#include "core/torrent.h"
#include "core/torrentinfo.h"
#include "global.h"
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>

TorrentInfoPanel::TorrentInfoPanel(QWidget *parent)
	: QTabWidget(parent)
{
	setFixedHeight(200);

	QWidget* infoTab = new QWidget;
	QVBoxLayout* infoLayout = new QVBoxLayout;
	infoLayout->addWidget(m_torrentName = new QLabel);
	infoLayout->addWidget(m_torrentSize = new QLabel);
	infoLayout->addWidget(m_pieceSize = new QLabel);
	infoLayout->addWidget(m_infoHash = new QLabel);
	infoLayout->addWidget(m_creationDate = new QLabel);
	infoLayout->addWidget(m_createdBy = new QLabel);
	infoLayout->addWidget(m_comment = new QLabel);
	infoTab->setLayout(infoLayout);

	addTab(infoTab, "Info");
}

void TorrentInfoPanel::refreshInfoTab()
{
	Torrent *torrent = QTorrent::instance()->mainWindow()->
					   torrentsList()->currentTorrent();
	if (!torrent) {
		m_torrentName->clear();
		m_torrentSize->clear();
		m_pieceSize->clear();
		m_infoHash->clear();
		m_creationDate->clear();
		m_createdBy->clear();
		m_comment->clear();
		return;
	}
	TorrentInfo *info = torrent->torrentInfo();

	m_torrentName->setText(tr("Name: %1").arg(
							   QString::fromUtf8(info->torrentName())));
	m_torrentSize->setText(tr("Total Size: %1 (%2 bytes)")
						   .arg(formatSize(info->length()))
						   .arg(info->length()));
	m_pieceSize->setText(tr("Piece size: %1 (%2 bytes)")
						 .arg(formatSize(info->pieceLength()))
						 .arg(info->pieceLength()));
	m_infoHash->setText(tr("Info hash: %1").arg(
							QString::fromUtf8(info->infoHash().toHex())));
	m_creationDate->setText(tr("Creation date: %1")
							.arg(info->creationDate()
								 ? info->creationDate()->toString()
								 : tr("Not available")));
	m_createdBy->setText(tr("Created by: %1")
						 .arg(info->createdBy()
							  ? *info->createdBy()
							  : tr("Not available")));
	m_comment->setText(tr("Comment: %1")
					   .arg(info->comment()
							? *info->comment()
							: tr("Not available")));
}

void TorrentInfoPanel::refresh()
{
	refreshInfoTab();
}

void TorrentInfoPanel::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	QAction *hide = new QAction(tr("Hide Info Panel"), &menu);
	menu.addAction(hide);
	connect(hide, &QAction::triggered, this, &TorrentInfoPanel::hide);
	menu.exec(event->globalPos());
}

void TorrentInfoPanel::hide()
{
	QWidget::hide();
	emit visibilityChanged(false);
}
