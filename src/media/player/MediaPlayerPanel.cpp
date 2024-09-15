#include "MediaPlayerPanel.h"

#include "VolumeController.h"
#include "EnhancedSlider.h"
#include "FullScreenButton.h"

#include <QPaintEvent>
#include <QMargins>
#include <QPainter>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>


namespace  {
	QSize textSize(const QString& text, const QFontMetrics& metrics) {
		return metrics.size(0, text);
	}

	QSize textSize(const QString& text, const QFont& font) {
		return text.isEmpty() ? QSize{} : textSize(text, QFontMetrics(font));
	}

	constexpr QMargins mediaPlayerPanelMargins = { 10, 5, 10, 5 };
	constexpr int mediaPlayerPanelBorderRadius = 10;
}


MediaPlayerPanel::MediaPlayerPanel(QWidget* parent):
	QWidget(parent) 
{
	setContentsMargins(0, 0, 0, 0);

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	_mediaPlayerPanelWidth = screenWidth / 4;

	_videoStateWidget = new VideoStateWidget(this);
	_volumeToggle = new VolumeController(this);

	_volumeSlider = new EnhancedSlider(this);
	_playbackSlider = new EnhancedSlider(this);

	_timeLabel = new QLabel(this);
	_remainingTimeLabel = new QLabel(this);

	_fullScreenButton = new FullScreenButton(this);

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

	_fullScreenButton->setFixedSize(25, 25);
	_videoStateWidget->setFixedSize(30, 30);
	_volumeToggle->setFixedSize(20, 20);

	_volumeSlider->setFixedHeight(20);
	_playbackSlider->setFixedHeight(20);

	connect(_volumeToggle, &QPushButton::clicked, this, [this]() {
		_volumeToggle->repaint();

		if (_volumeToggle->isSpeakerOn()) {
			_previousVolumeSliderValue = _volumeSlider->value();
			_volumeSlider->setSliderValue(0);
		}
		else {
			_volumeSlider->setSliderValue(_previousVolumeSliderValue);
		}
	});

	connect(_fullScreenButton, &QAbstractButton::clicked, this, [this]() {
		_fullScreenButton->state() == FullScreenButton::State::FullScreenTo ? emit mediaPlayerNeedsNormal() : emit mediaPlayerNeedsFullScreen();
	});

	connect(_videoStateWidget, &QPushButton::clicked, this, [this]() {
		switch (_videoStateWidget->state()) {

		case VideoStateWidget::State::Play:
			emit videoPlayClicked();
			break;
		case VideoStateWidget::State::Pause:
			emit videoPauseClicked();
			break;
		case VideoStateWidget::State::Repeat:
			emit videoRepeatClicked();
			break;
		}
		});
}

void MediaPlayerPanel::updateTimeText(int mediaPosition, int mediaDuration) {
	const auto positionSeconds = (mediaPosition / 1000) % 60;
	const auto positionMinutes = (mediaPosition / 1000) / 60;

	const auto durationSeconds = (mediaDuration / 1000) % 60;
	const auto durationMinutes = (mediaDuration / 1000) / 60;

	_timeLabel->setText(QString("%1:%2")
		.arg(positionMinutes, 2, 10, QChar('0'))
		.arg(positionSeconds, 2, 10, QChar('0')));

	_remainingTimeLabel->setText(QString("-%1:%2")
		.arg(durationMinutes - positionMinutes, 2, 10, QChar('0'))
		.arg(durationSeconds - positionSeconds, 2, 10, QChar('0')));

	updateTimeSize();
}

void MediaPlayerPanel::updateStateWidget(VideoStateWidget::State state) {
	_videoStateWidget->setState(state);
}

void MediaPlayerPanel::setVideoSliderMaximum(int value) {
	_playbackSlider->setMaximum(value);
}

void MediaPlayerPanel::updateSize() {
	const auto width = contentLeft() + _mediaPlayerPanelWidth + contentRight();
	const auto height = contentTop() + contentHeight() + contentBottom();

	resize(width, height);
}

void MediaPlayerPanel::updateTimeSize() {
	const auto timeLabelSize = textSize(_timeLabel->text(), _timeLabel->font());
	const auto remainingTimeLabelSize = textSize(_remainingTimeLabel->text(), _remainingTimeLabel->font());

	if (timeLabelSize.isNull() || remainingTimeLabelSize.isNull())
		return;

	_timeLabel->resize(timeLabelSize);
	_remainingTimeLabel->resize(remainingTimeLabelSize);
}

void MediaPlayerPanel::updateControlsGeometry() {
	_playbackSlider->resize(width() - _remainingTimeLabel->width() / 2.
					- _timeLabel->width() / 2., _playbackSlider->height());

	_playbackSlider->move(_timeLabel->width() / 2.,
			height() - contentBottom() - _playbackSlider->height());

	_volumeSlider->resize((width() - contentLeft() - contentRight()) / 5., _volumeSlider->height());
	_volumeSlider->move(contentLeft() * 1.5 + _volumeToggle->width(), contentTop());

	_volumeToggle->move(contentLeft(), contentTop());
	_videoStateWidget->move((width() - _videoStateWidget->width()) / 2., contentTop());

	_timeLabel->move(contentLeft(), height() + contentBottom() - _timeLabel->height());
	_remainingTimeLabel->move(width() + contentRight() - _remainingTimeLabel->width() / 2., height() + contentBottom() - _remainingTimeLabel->height());

	_fullScreenButton->move(width() - contentRight() - _fullScreenButton->width(), contentTop());
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
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.setOpacity(0.8);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);

	drawRoundedCorners(painter, mediaPlayerPanelBorderRadius);
}

void MediaPlayerPanel::resizeEvent(QResizeEvent* event) {
	updateSize();
	updateControlsGeometry();
}

void MediaPlayerPanel::mousePressEvent(QMouseEvent* event) {
	event->accept();
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

int MediaPlayerPanel::contentHeight() const noexcept {
	return _videoStateWidget->height() + _playbackSlider->height()
		+ contentTop() + contentBottom();
}
