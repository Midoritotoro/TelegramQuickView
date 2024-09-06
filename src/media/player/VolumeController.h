#pragma once


#include "../ClickableLabel.h"


class VolumeController: public ClickableLabel
{
	Q_OBJECT
public:
	VolumeController(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
};