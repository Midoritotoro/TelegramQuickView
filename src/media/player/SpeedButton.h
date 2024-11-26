#pragma once

#include <QWidget>

#include "EnhancedSlider.h"


class SpeedButtonOverlay: public QWidget {
	Q_OBJECT
private:
	EnhancedSlider* _speedController = nullptr;
	float _speed = 1.0f;
public:
	SpeedButtonOverlay(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
};


class SpeedButton: public QWidget {
	Q_OBJECT
private:
	SpeedButtonOverlay* _overlay = nullptr;

	QString _speedButtonImagePath;
	QPixmap _currentPixmap;
public:
	SpeedButton(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
};