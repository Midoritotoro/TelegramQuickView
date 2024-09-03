#include "MediaPlayer.h"

#include "MediaPlayerPanel.h"

namespace {
	constexpr int mediaPlayerPanelBottomIndent = 5;
}


MediaPlayer::MediaPlayer(QWidget* parent) :
	AbstractMediaPlayer(parent)
{
	setMouseTracking(true);

	_mediaPlayerPanel = new MediaPlayerPanel(this);
	_mediaPlayerPanel->show();
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	return _mediaPlayerPanel->height(); 
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	AbstractMediaPlayer::resizeEvent(event);

	_mediaPlayerPanel->move((width() - _mediaPlayerPanel->width()) / 2, height() - _mediaPlayerPanel->height() - mediaPlayerPanelBottomIndent);
}