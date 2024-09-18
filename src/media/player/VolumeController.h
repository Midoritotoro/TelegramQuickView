#pragma once

#include <QPushButton>

class EnhancedSlider;


class VolumeController: public QPushButton
{
	Q_OBJECT
private:
	EnhancedSlider* _volumeSlider = nullptr;
	bool _isSpeakerOn, _isVolumeValueSmall;
	int _previousVolume;
public:
	VolumeController(QWidget* parent = nullptr);

	[[nodiscard]] bool isSpeakerOn() const noexcept;

	void setSpeakerEnabled(bool enabled);
	void setVolume(int volume);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void paintSpeakerOff(QPainter& painter);
	void paintSpeakerOn(QPainter& painter);
};