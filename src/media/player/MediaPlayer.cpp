﻿#include "MediaPlayer.h"

#include <QMediaMetaData>
#include <QFont>
#include "../WidgetsHider.h"


namespace {
	QSize textSize(const QString& text, const QFontMetrics& metrics) {
		return metrics.size(0, text);
	}

	QSize textSize(const QString& text, const QFont& font) {
		return text.isEmpty() ? QSize{} : textSize(text, QFontMetrics(font));
	}

	constexpr int mediaPlayerPanelBottomIndent = 5;
}


MediaPlayer::MediaPlayer(QWidget* parent) :
	AbstractMediaPlayer(parent)
{
	_mediaPlayerPanel = new MediaPlayerPanel(this);

	_widgetsHider = new WidgetsHider(true);

	_widgetsHider->SetInactivityDuration(3000);
	_widgetsHider->SetAnimationDuration(3000);

	connect(mediaPlayer(), &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
		adjustVideoSize();

		if (status == QMediaPlayer::MediaStatus::EndOfMedia) {
			mediaPlayer()->pause();
			const auto duration = mediaPlayer()->duration();

			_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Repeat);
			videoRewind(duration - 100);
		}
		});

	connect(mediaPlayer(), &QMediaPlayer::durationChanged, _mediaPlayerPanel, &MediaPlayerPanel::setVideoSliderMaximum);
	connect(mediaPlayer(), &QMediaPlayer::positionChanged, this, [this]() {

		const auto position = mediaPlayer()->position();
		const auto duration = mediaPlayer()->duration();

		_mediaPlayerPanel->updateTimeText(position, duration);
	});

	connect(mediaPlayer(), &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
		switch (state) {

		case QMediaPlayer::PlaybackState::PlayingState: 
			_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Pause);
			break;

		case QMediaPlayer::PlaybackState::PausedState:
			const auto duration = mediaPlayer()->duration();
			const auto position = mediaPlayer()->position();

			if ((duration - position) > 100)
				_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Play);
			
			break;
			
		}
		});

	connect(this, &AbstractMediaPlayer::videoClicked, this, [this]() {

		switch (mediaPlayer()->playbackState()) {

		case QMediaPlayer::PlayingState:
			mediaPlayer()->pause();
			break;

		case QMediaPlayer::PausedState: 
			const auto duration = mediaPlayer()->duration();
			const auto position = mediaPlayer()->position();

			if ((duration - position) <= 100)
				videoRewind(0);

			mediaPlayer()->play();

			break;
		}
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoRepeatClicked, this, [this]() {
		videoRewind(0);
		mediaPlayer()->play();
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoPlayClicked, mediaPlayer(), &QMediaPlayer::play);
	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoPauseClicked, mediaPlayer(), &QMediaPlayer::pause);

	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsNormal, this, &AbstractMediaPlayer::mediaPlayerShowNormal);
	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsFullScreen, this, &AbstractMediaPlayer::mediaPlayerShowFullScreen);

	connect(this, &AbstractMediaPlayer::sourceChanged, this, &MediaPlayer::updateMediaPlayerPanelVisibility);
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	if (!mediaPlayer()->source().isEmpty())
		return _mediaPlayerPanel->height(); 
	return 0;
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	AbstractMediaPlayer::resizeEvent(event);

	_mediaPlayerPanel->move((width() - _mediaPlayerPanel->width()) / 2,
		height() - _mediaPlayerPanel->height() - mediaPlayerPanelBottomIndent);
}

void MediaPlayer::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.setRenderHints(QPainter::Antialiasing);
	painter.setOpacity(0.1);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);

	painter.drawRect(rect());
}

void MediaPlayer::updateMediaPlayerPanelVisibility(const QUrl& media) {
	QString sourcePath;

	media.path().at(0) == "/"[0]
		? sourcePath = media.path().remove(0, 1)
		: sourcePath = media.path();

	QString mediaType = detectMediaType(sourcePath);

	if (mediaType.contains("image")) {
		_widgetsHider->removeWidget(_mediaPlayerPanel);
		_widgetsHider->resetTimer();
		_mediaPlayerPanel->hide();
	}
	else if (mediaType.contains("video")) {
		_widgetsHider->addWidget(_mediaPlayerPanel);
		_widgetsHider->resetTimer();
		_mediaPlayerPanel->show();
	}
}
