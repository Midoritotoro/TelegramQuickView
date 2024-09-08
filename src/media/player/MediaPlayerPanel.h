#pragma once

#include <QWidget>
#include <QLabel>
#include "VideoStateWidget.h"

class VolumeController;
class EnhancedSlider;


class MediaPlayerPanel: public QWidget {
	Q_OBJECT
private:
	VideoStateWidget* _videoStateWidget = nullptr;
	EnhancedSlider* _playbackSlider = nullptr, *_volumeSlider = nullptr;
	QLabel* _timeLabel = nullptr, *_remainingTimeLabel = nullptr;

	VolumeController* _volumeToggle = nullptr;
	QPixmap _cache;

	int _previousVolumeSliderValue = 0;
public:
	MediaPlayerPanel(QWidget* parent = nullptr);

	void updateTimeText(int mediaPosition, int mediaDuration);
	void updateStateWidget(VideoStateWidget::State state);
	void setVideoSliderMaximum(int value);
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

	int contentWidth() const noexcept;
	int contentHeight() const noexcept;
};