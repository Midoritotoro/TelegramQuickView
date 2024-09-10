#pragma once

#include "AbstractMediaPlayer.h"
#include "MediaPlayerPanel.h"


class MediaPlayer : public AbstractMediaPlayer
{
private:
	Q_OBJECT
	MediaPlayerPanel* _mediaPlayerPanel = nullptr;
public:
	MediaPlayer(QWidget* parent = nullptr);

	[[nodiscard]] int getVideoControlsHeight() const noexcept;
	[[nodiscard]] MediaPlayerPanel* getVideoControls() const noexcept;
protected:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
}; 
