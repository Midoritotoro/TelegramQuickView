#pragma once

#include <QSlider>
#include <QPropertyAnimation>

#include "../core/ImagePushButton.h"

class VolumeButton : public ImagePushButton
{
private:
	Q_OBJECT
	QSlider* m_VolumeSlider = nullptr;
public:
	VolumeButton(const QImage& image, QSlider* VolumeSlider, QWidget* parent = nullptr);
protected:
	void hideEvent(QHideEvent* event) override;
};