#include "MediaPlayer.h"


MediaPlayer::MediaPlayer(QWidget* parent) :
	AbstractMediaPlayer(parent)
{
	setMouseTracking(true);
}