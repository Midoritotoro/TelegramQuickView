#include "MessageWidget.h"

#include "../media/ClickableLabel.h"
#include <Windows.h>
#include <cmath>
#include <QResizeEvent>

MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	QGridLayout* grid = new QGridLayout(this);

	panelWidth = screenWidth / 3;

	setFixedSize(panelWidth, screenHeight);

	setContentsMargins(0, 0, 0, 0);

	image = QImage("C:\\Users\\danya\\Downloads\\top.png");
	messageWidget = new QWidget();
	QGridLayout* gridLayout = new QGridLayout(messageWidget);

	grid->addWidget(messageWidget, 0, 0, 1, 1, Qt::AlignCenter);
	grid->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	imageLabel = new QLabel(this);
	imageLabel->setPixmap(QPixmap::fromImage(image));
	gridLayout->setAlignment(Qt::AlignCenter);
	imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	gridLayout->addWidget(imageLabel, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

}

QSize MessageWidget::getImageAspectRatio(const QImage& image) {
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

QSize MessageWidget::getMinimumSizeWithAspectRatio(const QSize& aspectRatio, const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageWidget::resizeEvent(QResizeEvent* event) {
	QSize size = getMinimumSizeWithAspectRatio(getImageAspectRatio(image), image.size(), panelWidth);
	QImage img2 = image.scaled(size, Qt::KeepAspectRatio);
	imageLabel->setPixmap(QPixmap::fromImage(img2));
	event->accept();
}