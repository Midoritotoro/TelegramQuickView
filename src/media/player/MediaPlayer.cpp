#include "MediaPlayer.h"

#include "MediaPlayerPanel.h"

namespace {
	constexpr int mediaPlayerPanelBottomIndent = 5;
}


MediaPlayer::MediaPlayer(QWidget* parent) :
	AbstractMediaPlayer(parent)
{
	_mediaPlayerPanel = new MediaPlayerPanel(this);
	_mediaPlayerPanel->show();

	connect(mediaPlayer(), &QMediaPlayer::durationChanged, _mediaPlayerPanel, &MediaPlayerPanel::setVideoSliderMaximum);
	connect(mediaPlayer(), &QMediaPlayer::positionChanged, this, [this]() {
		const auto position = mediaPlayer()->position();
		const auto duration = mediaPlayer()->position();

		_mediaPlayerPanel->updateTimeText(position, duration);
	});

	//connect(mediaPlayer(), &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
	//	adjustVideoSize();

	//	switch (state) {

	//	case QMediaPlayer::PlaybackState::PlayingState: 
	//		mediaPlayer()->pause();
	//		_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Play);
	//		break;

	//	case QMediaPlayer::PlaybackState::PausedState:
	//		const auto duration = mediaPlayer()->duration();
	//		const auto position = mediaPlayer()->duration();

	//		if ((duration - position) <= 100) {
	//			_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Repeat);
	//		}

	//		mediaPlayer()->play();
	//		_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Pause);
	//		break;
	//		
	//	}
	//	});
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	return _mediaPlayerPanel->height(); 
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	AbstractMediaPlayer::resizeEvent(event);

	_mediaPlayerPanel->move((width() - _mediaPlayerPanel->width()) / 2, height() - _mediaPlayerPanel->height() - mediaPlayerPanelBottomIndent);
}

void MediaPlayer::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setOpacity(0.3);

	painter.setBrush(Qt::black);
	painter.setPen(Qt::NoPen);

	painter.drawRect(rect());
}