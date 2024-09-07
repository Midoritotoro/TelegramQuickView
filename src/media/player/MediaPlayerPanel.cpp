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
	_volumeToggle = new VolumeController(this);

	_volumeSlider = new EnhancedSlider(this);
	_playbackSlider = new EnhancedSlider(this);

	_timeLabel = new QLabel(this);
	_remainingTimeLabel = new QLabel(this);

	_timeLabel->setAttribute(Qt::WA_NoSystemBackground);
	_remainingTimeLabel->setAttribute(Qt::WA_NoSystemBackground);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString sliderStylePath = cssDir.absolutePath() + "/SliderStyle.css";

	QFile sliderStyleFile(sliderStylePath);
	if (sliderStyleFile.open(QFile::ReadOnly)) {

		QByteArray sliderStyle = sliderStyleFile.readAll();

		_playbackSlider->setStyleSheet(sliderStyle);
		_volumeSlider->setStyleSheet(sliderStyle);

		sliderStyleFile.close();
	}

	_videoStateWidget->resize(20, 20);

	_volumeSlider->setFixedHeight(20);
	_playbackSlider->setFixedHeight(20);
	_volumeToggle->resize(20, 20);

	resize(width(), _videoStateWidget->height() + _playbackSlider->height()
					+ _volumeSlider->height() + _volumeToggle->height()
					+ contentTop() + contentBottom());

	_volumeSlider->show();
	_volumeToggle->show();
	_videoStateWidget->show();
	_playbackSlider->show();

	updateSize();

	connect(_volumeToggle, &QAbstractButton::clicked, this, [this]() {
		_volumeToggle->repaint();

		if (_volumeToggle->isSpeakerOn()) {
			_previousVolumeSliderValue = _volumeSlider->value();
			_volumeSlider->setSliderValue(0);
		}
		else {
			_volumeSlider->setSliderValue(_previousVolumeSliderValue);
		}
	});
}

void MediaPlayerPanel::updateTimeText(int mediaPosition, int mediaDuration) {
	const auto positionSeconds = (mediaPosition / 1000) % 60;
	const auto positionMinutes = (mediaPosition / 1000) / 60;
	
	const auto durationSeconds = (mediaPosition / 1000) % 60;
	const auto durationMinutes = (mediaDuration / 1000) / 60;

	_timeLabel->setText(QString("%1:%2 / %3:%4")
				.arg(positionMinutes, 2, 10, QChar('0'))
				.arg(positionSeconds, 2, 10, QChar('0'))
				.arg(durationSeconds)
				.arg(durationMinutes));
}

void MediaPlayerPanel::updateSize() {
	const auto width = contentLeft() + mediaPlayerPanelWidth + contentRight();
	const auto height = contentTop() + contentHeight() + contentBottom();

	_timeLabel->adjustSize();
	_remainingTimeLabel->adjustSize();

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

	_playbackSlider->resize(width() - contentLeft() - contentRight() - _timeLabel->width() 
							- _remainingTimeLabel->width(), _playbackSlider->height());

	_volumeSlider->resize((width() - contentLeft() - contentRight()) / 5, _volumeSlider->height());

	_playbackSlider->move(contentLeft() + _timeLabel->width(), height() - contentBottom() - _playbackSlider->height());
	_volumeSlider->move(contentLeft() * 1.5 + _volumeToggle->width(), contentTop());

	_volumeToggle->move(contentLeft(), contentTop());
	_videoStateWidget->move((width() - _videoStateWidget->width()) / 2, contentTop());

	_timeLabel->move(contentLeft(), height() - contentBottom() - _timeLabel->height());
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