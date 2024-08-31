#include "MediaPlayerPanel.h"

#include <QPaintEvent>
#include <QMargins>
#include <QPainter>


namespace {
	constexpr QMargins mediaPlayerPanelMargins = { 10, 10, 10, 10 };
	constexpr int mediaPlayerPanelWidth = 344;
	constexpr int mediaPlayerPanelHeight = 288;
}

MediaPlayerPanel::MediaPlayerPanel(QWidget* parent):
	QWidget(parent) 
{
	hide();
	updateSize();
}

void MediaPlayerPanel::updateSize() {
	int width = mediaPlayerPanelMargins.left() + mediaPlayerPanelWidth + mediaPlayerPanelMargins.right();
	int height = mediaPlayerPanelMargins.top();

	height += mediaPlayerPanelHeight + mediaPlayerPanelMargins.bottom();
	resize(width, height);
}

void MediaPlayerPanel::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawPixmap(0, 0, QPixmap());
	raise();
}