#include "MessageWidget.h"

#include <Windows.h>
#include <cmath>
#include <QResizeEvent>


MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	_mediaPlayer = new MediaPlayer();

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
		"background: blue;\n"
		"border: 5px;\n"
		"}");

	grid->addWidget(messageWidget, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
	grid->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	imageLabel = new ClickableLabel(this);
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

	messageWidget->setMouseTracking(true);

	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	imageLabel->setCursor(Qt::PointingHandCursor);

	connect(imageLabel, &ClickableLabel::clicked, this, &MessageWidget::attachment_cliked);
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

void MessageWidget::attachment_cliked() {
	_mediaPlayer->setSource(QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\top.png"));
	_mediaPlayer->exec();
}