#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

class MessageWidget : public QWidget {
private:
	Q_OBJECT
	QLabel* imageLabel;
	int panelWidth;
	QImage image;
	QWidget* messageWidget;
public:
	MessageWidget(QWidget* parent = nullptr);
	[[nodiscard]] static QSize getImageAspectRatio(const QImage& image);
	[[nodiscard]] QSize getMinimumSizeWithAspectRatio(const QSize& aspectRatio, const QSize& imageSize, const int parentWidth);
protected:
	void resizeEvent(QResizeEvent* event) override;
};