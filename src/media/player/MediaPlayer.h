#pragma once

#include "AbstractMediaPlayer.h"

class MediaPlayerPanel;

class MediaPlayer : public AbstractMediaPlayer
{
private:
	Q_OBJECT
	MediaPlayerPanel* _mediaPlayerPanel = nullptr;
public:
	MediaPlayer(QWidget* parent = nullptr);
	[[nodiscard]] int getVideoControlsHeight() const noexcept;
protected:
	void resizeEvent(QResizeEvent* event) override;
}; 
