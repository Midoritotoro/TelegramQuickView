#include "MessageWidget.h"

#include <Windows.h>
#include <cmath>
#include <QResizeEvent>

MessageAttachment::MessageAttachment(QString attachmentPath):
	_attachmentPath(attachmentPath), _attachmentType(MediaPlayer::detectMediaType(attachmentPath))
{}

QString MessageAttachment::attachmentPath() const {
	return _attachmentPath;
}

QString MessageAttachment::attachmentType() const {
	return _attachmentType;
}

MessageAttachmentImage::MessageAttachmentImage(QString attachmentPath, QWidget* parent):
	ClickableLabel(parent), MessageAttachment(attachmentPath)
{

}

MessageAttachmentVideo::MessageAttachmentVideo(QString attachmentPath, QWidget* parent):
	ClickableLabel(parent), MessageAttachment(attachmentPath)
{

}

MessageWidget::MessageWidget(QWidget* parent) :
	QWidget(parent)
{

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	_mediaPlayer = new MediaPlayer();

	QGridLayout* grid = new QGridLayout(this);

	QLabel* textLabel = new QLabel();
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

	
	messageWidget = new QWidget();
	gridLayout = new QGridLayout(messageWidget);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: darkblue;\n"
		"border: 5px;\n"
		"}");

	grid->addWidget(messageWidget, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
	grid->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	
	gridLayout->setAlignment(Qt::AlignCenter);
	gridLayout->setVerticalSpacing(0);

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	gridLayout->addWidget(textLabel, 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);

	messageWidget->setMouseTracking(true);

	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void MessageWidget::setSource(const QString& messageText, const QUrlList& attachmentsPaths) {
	foreach(const QUrl& url, attachmentsPaths) {
		if (MediaPlayer::detectMediaType(url.path()) == "image") {
			image = QImage(url.path());
			image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
			imageLabel = new ClickableLabel(this);
			imageLabel->setScaledContents(true);
			imageLabel->setBackgroundRole(QPalette::Dark);
			imageLabel->setPixmap(QPixmap::fromImage(image));
			imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			gridLayout->addWidget(imageLabel, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
			imageLabel->setCursor(Qt::PointingHandCursor);

			connect(imageLabel, &MessageAttachmentImage::clicked, this, &MessageWidget::attachment_cliked);
		}
	}
}

void MessageWidget::setSource(const QString & messageText, const QUrl & attachmentPath) {
	QUrlList urlList = { attachmentPath };
	setSource(messageText, urlList);
}

QSize MessageWidget::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageWidget::resizeEvent(QResizeEvent* event) {
	if (image == QImage())
		return;
	QSize size = getMinimumSizeWithAspectRatio( image.size(), panelWidth);
	QImage img = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	imageLabel->setPixmap(QPixmap::fromImage(img));
	event->accept();
}

void MessageWidget::attachment_cliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();	
	if (attachment->attachmentType() == "image") {
		_mediaPlayer->setSource(QUrl::fromLocalFile(attachment->attachmentPath()));
		_mediaPlayer->exec();
	}
}