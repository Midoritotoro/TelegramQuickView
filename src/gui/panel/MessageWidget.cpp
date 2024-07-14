#include "MessageWidget.h"

#include "../media/ClickableLabel.h"
#include <Windows.h>
#include <cmath>

MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int panelWidth = screenWidth / 3;

	setFixedSize(screenWidth, screenHeight);

	setContentsMargins(0, 0, 0, 0);
	setStyleSheet("QWidget{ \n"
		"background: Wheat;\n"
	"}");

	//QImage image("D:\\Media\\chatgpt3\\1\\Изображения\\photo_2024-07-12_09-49-17.jpg");
	//if (image.width() > panelWidth || image.height() > screenHeight) {
	//	QSize aspectRatio = getImageAspectRatio(image);

	//	image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	//	image.
	//	QLabel* imageLabel = new QLabel(this);
	//	imageLabel->setPixmap(QPixmap::fromImage(image));
	//	imageLabel->setGeometry(0, 0, panelWidth, 700);
	//	QPainter painter(imageLabel);
	//	//painter.setRenderHint(QPainter::Antialiasing, true);
	//
	//	painter.drawImage(QRect(0, 0, panelWidth, 700), image);
	//}

}

QSize MessageWidget::getImageAspectRatio(QImage& image) {
	int width = image.width();
	int height = image.height();

	double imageAspectRatioWidthToHeight = (double(width) / double(height)) * 1000;
	double imageAspectRatioHeightToWidth = (double(height) / double(width)) * 1000;

	int intAspectRatioWidthToHeight = static_cast<int>(imageAspectRatioWidthToHeight);
	int intAspectRatioHeightToWidth = static_cast<int>(imageAspectRatioHeightToWidth);

	int gcd = std::gcd(intAspectRatioWidthToHeight, intAspectRatioHeightToWidth);
	imageAspectRatioWidthToHeight = std::ceil(imageAspectRatioWidthToHeight / (gcd * 100));
	imageAspectRatioHeightToWidth = std::ceil(imageAspectRatioHeightToWidth / (gcd * 100));

	return QSize(imageAspectRatioWidthToHeight, imageAspectRatioHeightToWidth);
}