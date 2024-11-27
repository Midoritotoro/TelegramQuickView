#pragma once

#include <QWidget>
#include <QPushButton>

#include <functional>

#include "EnhancedSlider.h"


class SpeedButtonOverlay: public QWidget {
	Q_OBJECT
private:
	EnhancedSlider* _speedSlider = nullptr;
	QRect _textRect;

	std::function<void (float speed)> _onSpeedChangedCallback;

	float _speed = 1.0f;
public:
	SpeedButtonOverlay(QWidget* parent = nullptr);
	
	void setCallback(std::function<void (float speed)> callback);
	[[nodiscard]] float speed() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
};


class SpeedController: public QPushButton {
	Q_OBJECT
private:
	SpeedButtonOverlay* _overlay = nullptr;

	QString _speedButtonImagePath;
	QPixmap _currentPixmap;
public:
	SpeedController(QWidget* parent = nullptr);

	void setCallback(std::function<void (float speed)> callback);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

	bool eventFilter(QObject* sender, QEvent* event) override;
};