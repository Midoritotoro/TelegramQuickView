#pragma once

#include <QPushButton>

class EnhancedSlider;


class VolumeController: public QPushButton
{
	Q_OBJECT
private:
	EnhancedSlider* _volumeSlider = nullptr;
	bool _isSpeakerOn;
	int _previousVolume;
public:
	VolumeController(QWidget* parent = nullptr);

	[[nodiscard]] bool isSpeakerOn() const noexcept;
	void setSpeakerEnabled(bool enabled);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void paintSpeakerOff(QPainter& painter);
	void paintSpeakerOn(QPainter& painter);
};