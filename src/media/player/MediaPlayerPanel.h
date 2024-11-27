#pragma once

#include <QWidget>
#include <QLabel>

#include "VideoStateWidget.h"
#include "EnhancedSlider.h"

class VolumeController;
class FullScreenButton;
class SpeedController;


class MediaPlayerPanel: public QWidget {
	Q_OBJECT
private:
	VideoStateWidget* _videoStateWidget = nullptr;
	EnhancedSlider* _playbackSlider = nullptr, *_volumeSlider = nullptr;
	QLabel* _timeLabel = nullptr, *_remainingTimeLabel = nullptr;

	FullScreenButton *_fullScreenButton = nullptr;
	SpeedController* _speedButton = nullptr;

	QString _fullScreenToImagePath;

	VolumeController* _volumeToggle = nullptr;
	int _previousVolumeSliderValue = 0, _mediaPlayerPanelWidth = 0;
public:
	MediaPlayerPanel(QWidget* parent = nullptr);

	void updateTimeText(int mediaPosition, int mediaDuration);
	void updateStateWidget(VideoStateWidget::State state);

	[[nodiscard]] EnhancedSlider* volumeSlider() const noexcept;
	[[nodiscard]] EnhancedSlider* playbackSlider() const noexcept;

	void setVideoSliderMaximum(int value);
	void setVolume(int value);
Q_SIGNALS:
	void videoRepeatClicked();
	void videoPlayClicked();
	void videoPauseClicked();

	void mediaPlayerNeedsFullScreen();
	void mediaPlayerNeedsNormal();
	void mediaPlayerNeedsChangeVolume(int value);
protected:
	void drawRoundedCorners(QPainter& painter, int borderRadius);

	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
private:
	void updateSize();
	void updateTimeSize();

	void updateControlsGeometry();

	int contentHeight() const noexcept;
};