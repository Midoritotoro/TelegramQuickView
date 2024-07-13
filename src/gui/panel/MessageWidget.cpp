#include "MessageWidget.h"

#include "../media/ClickableLabel.h"

MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{
	setFixedSize(1920, 1080);
	setContentsMargins(0, 0, 0, 0);
	setStyleSheet("QWidget{ \n"
		"background: Wheat;\n"
	"}");
	QImage image("D:\\Media\\chatgpt3\\1\\Изображения\\photo_2024-07-12_09-49-17.jpg");
	image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	QLabel* imageLabel = new QLabel(this);
	imageLabel->setPixmap(QPixmap::fromImage(image));
	imageLabel->setGeometry(0, 0, width(), height());
	QPainter painter(imageLabel);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawImage(0, 0, image);

}