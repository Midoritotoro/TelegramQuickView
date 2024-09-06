#include "MediaPlayerPanel.h"

#include "VideoStateWidget.h"
#include "VolumeController.h"
#include "EnhancedSlider.h"

#include <QPaintEvent>
#include <QMargins>
#include <QPainter>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPainterPath>


namespace {
	constexpr QMargins mediaPlayerPanelMargins = { 10, 5, 10, 5 };
	constexpr int mediaPlayerPanelWidth = 344;
	constexpr int mediaPlayerPanelBorderRadius = 10;
}


MediaPlayerPanel::MediaPlayerPanel(QWidget* parent):
	QWidget(parent) 
{
	setContentsMargins(0, 0, 0, 0);

	_videoStateWidget = new VideoStateWidget(this);
	_volumeSlider = new EnhancedSlider(this);
	_playbackSlider = new EnhancedSlider(this);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString videoSliderStylePath = cssDir.absolutePath() + "/VideoSliderStyle.css";

	QFile videoSliderStyleFile(videoSliderStylePath);
	if (videoSliderStyleFile.open(QFile::ReadOnly)) {

		QByteArray videoSliderStyle = videoSliderStyleFile.readAll();
		_playbackSlider->setStyleSheet(videoSliderStyle);

		videoSliderStyleFile.close();
	}

	_videoStateWidget->resize(30, 30);

	_volumeSlider->setFixedHeight(20);
	_playbackSlider->setFixedHeight(20);

	resize(width(), height() + _videoStateWidget->height() + _playbackSlider->height());

	_volumeSlider->hide();
	_videoStateWidget->show();
	_playbackSlider->show();

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

	resize(width, height);
}

void MediaPlayerPanel::drawRoundedCorners(QPainter& painter, int borderRadius) {
	QPainterPath path;

	path.moveTo(borderRadius, 0);

	path.lineTo(width() - borderRadius, 0);
	path.quadTo(width(), 0, width(), borderRadius);

	path.lineTo(width(), height() - borderRadius);
	path.quadTo(width(), height(), width() - borderRadius, height());

	path.lineTo(borderRadius, height());
	path.quadTo(0, height(), 0, height() - borderRadius);

	path.lineTo(0, borderRadius);
	path.quadTo(0, 0, borderRadius, 0);

	painter.drawPath(path);

	QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);

	painter.setTransform(mirror);
	painter.translate(-width(), 0);

	painter.drawPath(path);
}

void MediaPlayerPanel::paintEvent(QPaintEvent* event) {
	QWidget::paintEvent(event);

	updateSize();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);

	drawRoundedCorners(painter, mediaPlayerPanelBorderRadius);
}

void MediaPlayerPanel::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);

	updateSize();

	_playbackSlider->resize(width() - contentLeft() - contentRight(), _playbackSlider->height());

	_videoStateWidget->move((width() - _videoStateWidget->width()) / 2, contentTop());
	_playbackSlider->move(contentLeft(),  height() - contentBottom() - _playbackSlider->height());
}