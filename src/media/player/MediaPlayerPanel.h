#pragma once

#include <QWidget>
#include <QLabel>

#include "VideoStateWidget.h"


class VolumeController;
class EnhancedSlider;
class FullScreenButton;



class MediaPlayerPanel: public QWidget {
	Q_OBJECT
private:
	VideoStateWidget* _videoStateWidget = nullptr;
	EnhancedSlider* _playbackSlider = nullptr, *_volumeSlider = nullptr;
	QLabel* _timeLabel = nullptr, *_remainingTimeLabel = nullptr;

	FullScreenButton *_fullScreenButton = nullptr;
	QString _fullScreenToImagePath;

	VolumeController* _volumeToggle = nullptr;
	int _previousVolumeSliderValue = 0, _mediaPlayerPanelWidth = 0;
public:
	MediaPlayerPanel(QWidget* parent = nullptr);

	void updateTimeText(int mediaPosition, int mediaDuration);
	void updateStateWidget(VideoStateWidget::State state);

	void setVideoSliderMaximum(int value);
Q_SIGNALS:
	void videoRepeatClicked();
	void videoPlayClicked();
	void videoPauseClicked();

	void mediaPlayerNeedsFullScreen();
	void mediaPlayerNeedsNormal();
protected:
	void drawRoundedCorners(QPainter& painter, int borderRadius);

	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void updateSize();
	void updateTimeSize();

	void updateControlsGeometry();

	int contentLeft() const noexcept;
	int contentTop() const noexcept;

	int contentRight() const noexcept;
	int contentBottom() const noexcept;

	int contentHeight() const noexcept;
};