#pragma once

#include "AbstractMediaPlayer.h"
#include "MediaPlayerPanel.h"

class WidgetsHider;


class MediaPlayer : public AbstractMediaPlayer
{
private:
	Q_OBJECT
	MediaPlayerPanel* _mediaPlayerPanel = nullptr;
	WidgetsHider* _widgetsHider = nullptr;
public:
	MediaPlayer(QWidget* parent = nullptr);

	[[nodiscard]] int getVideoControlsHeight() const noexcept;
protected:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	void updateMediaPlayerPanelVisibility(const QUrl& media);
}; 
