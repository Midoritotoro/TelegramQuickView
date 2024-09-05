#include "MediaPlayerPanel.h"

#include "VideoStateWidget.h"
#include "VolumeController.h"
#include "EnhancedSlider.h"

#include <QPaintEvent>
#include <QMargins>
#include <QPainter>


namespace {
	constexpr QMargins mediaPlayerPanelMargins = { 10, 10, 10, 10 };
	constexpr int mediaPlayerPanelWidth = 344;
}


MediaPlayerPanel::MediaPlayerPanel(QWidget* parent):
	QWidget(parent) 
{
	_videoStateWidget = new VideoStateWidget(this);
	_volumeSlider = new EnhancedSlider();
	_playbackSlider = new EnhancedSlider();

	_videoStateWidget->resize(50, 50);
	_videoStateWidget->setState(VideoStateWidget::State::Pause);
	_videoStateWidget->show();

	hide();
	updateSize();
}

int MediaPlayerPanel::contentLeft() const noexcept {	
	return mediaPlayerPanelMargins.left();
}

int MediaPlayerPanel::contentTop() const noexcept {
	return mediaPlayerPanelMargins.top();
}

int MediaPlayerPanel::contentRight() const noexcept {
	return mediaPlayerPanelMargins.right();
}

int MediaPlayerPanel::contentBottom() const noexcept {
	return mediaPlayerPanelMargins.bottom();
}

int MediaPlayerPanel::contentWidth() const noexcept {
	return width() - contentLeft() - contentRight();
}

int MediaPlayerPanel::contentHeight() const noexcept {
	return height() - contentTop() - contentBottom();
}

void MediaPlayerPanel::updateSize() {
	const auto width = contentLeft() + mediaPlayerPanelWidth + contentRight();
	const auto height = contentTop() + contentHeight() + contentBottom();
	
	adjustSize();
	resize(width, height);
}

void MediaPlayerPanel::paintEvent(QPaintEvent* event) {
	QWidget::paintEvent(event);

	qDebug() << "MediaPlayerPanel::paintEvent";

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);

	QPixmap pixmap(width(), height());
	pixmap.fill(Qt::black);

	painter.drawPixmap(0, 0, pixmap);
	//updateSize();
}

void MediaPlayerPanel::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);

	//updateSize();
	//_videoStateWidget->move((width() - _videoStateWidget->width()) / 2, height() - contentBottom());
}