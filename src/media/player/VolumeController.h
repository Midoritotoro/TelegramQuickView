#pragma once

#include <QAbstractButton>


class VolumeController: public QAbstractButton
{
	Q_OBJECT
private:
	bool _isSpeakerOn;
public:
	VolumeController(QWidget* parent = nullptr);

	[[nodiscard]] bool isSpeakerOn() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void paintSpeakerOff(QPainter& painter);
	void paintSpeakerOn(QPainter& painter);
};