#pragma once

#include <QPushButton>


class VolumeController: public QPushButton
{
	Q_OBJECT
private:
	bool _isSpeakerOn;
public:
	VolumeController(QWidget* parent = nullptr);

	[[nodiscard]] bool isSpeakerOn() const noexcept;
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	void paintSpeakerOff(QPainter& painter);
	void paintSpeakerOn(QPainter& painter);
};