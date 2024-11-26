#pragma once

#include <QWidget>

class SpeedButtonLayout;

class SpeedButton : public QWidget {
	Q_OBJECT
private:
	QString _speedButtonImagePath;
	QPixmap _currentPixmap;
public:
	SpeedButton(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
};