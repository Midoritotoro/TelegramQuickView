#include "MediaPlayer.h"

#include "MediaPlayerPanel.h"


MediaPlayer::MediaPlayer(QWidget* parent) :
	AbstractMediaPlayer(parent)
{
	_mediaPlayerPanel = new MediaPlayerPanel(this);
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	return _mediaPlayerPanel->height(); 
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	AbstractMediaPlayer::resizeEvent(event);
	_mediaPlayerPanel->move(width() / 2 - _mediaPlayerPanel->width(), _mediaPlayerPanel->pos().y());
}