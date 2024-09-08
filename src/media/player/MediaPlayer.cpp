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
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	return _mediaPlayerPanel->height(); 
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	AbstractMediaPlayer::resizeEvent(event);

	_mediaPlayerPanel->move((width() - _mediaPlayerPanel->width()) / 2, height() - _mediaPlayerPanel->height() - mediaPlayerPanelBottomIndent);
}