#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

class MessageWidget : public QWidget {
private:
	Q_OBJECT
public:
	MessageWidget(QWidget* parent = nullptr);
	[[nodiscard]] static QSize getImageAspectRatio(QImage& image);
};