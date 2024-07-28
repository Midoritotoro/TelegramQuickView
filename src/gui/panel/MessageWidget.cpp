#include "MessageWidget.h"

#include "../media/ClickableLabel.h"
#include <Windows.h>
#include <cmath>
#include <QResizeEvent>
#include <QTextEdit>


MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	QGridLayout* grid = new QGridLayout(this);

	QLabel* textLabel = new QLabel("dgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWidgetmessageWid");

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-family: centry gothic;\n"
		"font-size: 20px;\n"
		"}"));

	textLabel->setStyleSheet("QLabel{\n"
		"background: transparent;\n"
		"color: white;\n"
		"}");

	textLabel->setAttribute(Qt::WA_NoSystemBackground);
	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

	panelWidth = screenWidth / 3;
	setFixedSize(panelWidth, screenHeight);
	setContentsMargins(0, 0, 0, 0);

	image = QImage("C:\\Users\\danya\\Downloads\\top.png");
	messageWidget = new QWidget();
	QGridLayout* gridLayout = new QGridLayout(messageWidget);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: Wheat;\n"
		"border: 5px;\n"
	"}");
	

	grid->addWidget(messageWidget, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
	grid->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	imageLabel = new QLabel(this);
	imageLabel->setScaledContents(true);
	imageLabel->setBackgroundRole(QPalette::Dark);
	imageLabel->setPixmap(QPixmap::fromImage(image));

	gridLayout->setAlignment(Qt::AlignCenter);
	gridLayout->setVerticalSpacing(0);

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	gridLayout->addWidget(imageLabel, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
	gridLayout->addWidget(textLabel, 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);
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

QSize MessageWidget::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageWidget::resizeEvent(QResizeEvent* event) {
	QSize size = getMinimumSizeWithAspectRatio( image.size(), panelWidth);
	QImage img = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	imageLabel->setPixmap(QPixmap::fromImage(img));
	event->accept();
}