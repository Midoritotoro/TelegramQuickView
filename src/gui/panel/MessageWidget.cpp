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


	QWidget* textWidget = new QWidget();

	textWidget->setStyleSheet("QWidget{\n"
		"border-width: 15px; \n"
		"border-color: white;\n"
		"background: black;\n"
		"}");

	QGridLayout* textLayout = new QGridLayout(textWidget);
	QLabel* textLabel = new QLabel(" на API продуkeorweorkwep fpijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfjjaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oijaifjdasifjsafdoijsadfjaifjis jfjda foijfiasd jfoidjfaodfj oidfjaiofjdoi fjdfi jdafj oi oidfjaiofjdoi fjdfi jdafj oidsfjodisfj oaisdfjsoidfj isdfjisdjfisajf iadsfj isaodfjктов Microsoft;*код… Читать ещё.");

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-family: centry gothic;\n"
		"font-size: 24px;\n"
		"}"));

	textLabel->setStyleSheet("QLabel{\n"
		"background: Wheat;\n"
		"color: black;\n"
		"}");
	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	textLayout->setAlignment(Qt::AlignCenter);

	textLayout->addWidget(textLabel, 0, 0, 1, 1);

	panelWidth = screenWidth / 3;
	setFixedSize(panelWidth, screenHeight);
	setContentsMargins(0, 0, 0, 0);

	image = QImage("C:\\Users\\danya\\Downloads\\top.png");
	messageWidget = new QWidget();
	QGridLayout* gridLayout = new QGridLayout(messageWidget);
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
	textWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	gridLayout->addWidget(imageLabel, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
	gridLayout->addWidget(textWidget, 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);
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