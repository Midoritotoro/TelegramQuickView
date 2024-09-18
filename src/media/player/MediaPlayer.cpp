#include "MediaPlayer.h"

#include "../WidgetsHider.h"
#include <QAudioOutput>


namespace {
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
			videoRewind(duration - 1);
		}
		});

	connect(mediaPlayer(), &QMediaPlayer::durationChanged, _mediaPlayerPanel, &MediaPlayerPanel::setVideoSliderMaximum);
	connect(mediaPlayer(), &QMediaPlayer::positionChanged, this, [this](qint64 position) {
		const auto duration = mediaPlayer()->duration();

		_mediaPlayerPanel->playbackSlider()->setValue(position);
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

	connect(_mediaPlayerPanel->playbackSlider(), &QSlider::sliderPressed, [this]() {
		disconnect(mediaPlayer(), &QMediaPlayer::positionChanged, _mediaPlayerPanel->playbackSlider(), &QSlider::setValue);
		mediaPlayer()->pause();
		});

	connect(_mediaPlayerPanel->playbackSlider(), &QSlider::sliderReleased, [this]() {
		connect(mediaPlayer(), &QMediaPlayer::positionChanged, _mediaPlayerPanel->playbackSlider(), &QSlider::setValue);
		_sleep(1);
		mediaPlayer()->play();
		});

	connect(_mediaPlayerPanel->playbackSlider(), &QSlider::sliderMoved, this, &MediaPlayer::videoRewind);
	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsChangeVolume, this, &AbstractMediaPlayer::changeVolume);

	_mediaPlayerPanel->setVolume(20);
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	if (mediaPlayer()->source().isEmpty() == false)
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

	MediaType mediaType = detectMediaType(sourcePath);

	switch (mediaType) {
		case MediaType::Video:
			_widgetsHider->addWidget(_mediaPlayerPanel);
			_widgetsHider->resetTimer();

			_mediaPlayerPanel->show();
			break;

		case MediaType::Image:
			_widgetsHider->removeWidget(_mediaPlayerPanel);
			_widgetsHider->resetTimer();

			_mediaPlayerPanel->hide();
			break;

		case MediaType::Audio:
			break;
	}
	
}
