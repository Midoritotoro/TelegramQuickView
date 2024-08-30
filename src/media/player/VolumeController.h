#pragma once

#include <QSlider>

#include "../ClickableLabel.h"

class VolumeController: public ClickableLabel
{
private:
	Q_OBJECT
	QSlider* m_VolumeSlider = nullptr;
public:
	VolumeController(const QString& imagePath, QSlider* VolumeSlider, QWidget* parent = nullptr);
protected:
	void hideEvent(QHideEvent* event) override;
};