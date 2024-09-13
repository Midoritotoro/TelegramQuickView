#include "MediaPlayer.h"

#include <QMediaMetaData>
#include <QFont>


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

	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoPlayClicked, this, [this]() {
		mediaPlayer()->play();
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoPauseClicked, this, [this]() {
		mediaPlayer()->pause();
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsNormal, this, [this]() {
		qDebug() << "MediaPlayerPanel::mediaPlayerNeedsNormal";
		mediaPlayerShowNormal();
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsFullScreen, this, [this]() {
		qDebug() << "MediaPlayerPanel::mediaPlayerNeedsFullScreen";
		mediaPlayerShowFullScreen();
		});
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	if (!mediaPlayer()->source().isEmpty())
		return _mediaPlayerPanel->height(); 
	return 0;
}

MediaPlayerPanel* MediaPlayer::getVideoControls() const noexcept {
	return _mediaPlayerPanel;
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	AbstractMediaPlayer::resizeEvent(event);

	_mediaPlayerPanel->move((width() - _mediaPlayerPanel->width()) / 2,
		height() - _mediaPlayerPanel->height() - mediaPlayerPanelBottomIndent);
}

void MediaPlayer::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);
	painter.setOpacity(0.3);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);

	painter.drawRect(rect());
}
