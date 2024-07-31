﻿#include "MessageWidget.h"

#include <Windows.h>
#include <cmath>
#include <QResizeEvent>
#include <QStyleOptionTitleBar>
#include <QFile>
#include <QScrollBar>


MessageAttachment::MessageAttachment(QString attachmentPath, QWidget* parent) :
	ClickableLabel(parent), _attachmentPath(attachmentPath), _attachmentType(MediaPlayer::detectMediaType(attachmentPath))
{
	setMouseTracking(true);
	setScaledContents(true);
	setBackgroundRole(QPalette::Dark);
	setCursor(Qt::PointingHandCursor);

	if (_attachmentType.contains("image")) {
		QImage attachmentImage(attachmentPath);
		attachmentImage = attachmentImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		setPixmap(QPixmap::fromImage(attachmentImage));
	}
	else if (_attachmentType.contains("video")) {
		QImage attachmentVideoPreview = MediaPlayer::videoPreview(attachmentPath);
		attachmentVideoPreview = attachmentVideoPreview.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		setPixmap(QPixmap::fromImage(attachmentVideoPreview));
	}
}

QString MessageAttachment::attachmentPath() const {
	return _attachmentPath;
}

QString MessageAttachment::attachmentType() const {
	return _attachmentType;
}

MessageWidget::MessageWidget(const QString& messageText, const QUrlList& attachmentsPaths, QWidget* parent) :
	QWidget(parent)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	QWidget* chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QGridLayout(chatScrollAreaWidget);
	_chatScrollArea = new QScrollArea();
	_chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(0, 0, 0, 0);
	chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	_chatScrollArea->setContentsMargins(0, 0, 0, 0);
	_chatScrollAreaLayout->setHorizontalSpacing(0);

	chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	_chatScrollArea->setWidget(chatScrollAreaWidget);

	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString toolButtonStylePath = cssDir.absolutePath() + "/ToolButtonStyle.css";

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	QFile toolButtonStyleFile(toolButtonStylePath);
	if (toolButtonStyleFile.open(QFile::ReadOnly)) {

		QByteArray style = toolButtonStyleFile.readAll();
		closeWindowButton->setStyleSheet(style);
		minimizeWindowButton->setStyleSheet(style);

		toolButtonStyleFile.close();
	}

	_mediaPlayer = new MediaPlayer();
	QGridLayout* grid = new QGridLayout(this);

	QWidget* toolWidget = new QWidget();
	QGridLayout* toolLayout = new QGridLayout(toolWidget);
	toolWidget->setObjectName("toolWidget");
	toolWidget->setStyleSheet("#toolWidget{\n"
		"background: rgba(36, 47, 61, 1);\n"
	"}");
	toolWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	toolWidget->setContentsMargins(0, 0, 0, 0);
	toolLayout->setContentsMargins(0, 0, 0, 0);
	toolLayout->setSpacing(0);

	toolLayout->addWidget(minimizeWindowButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(closeWindowButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);

	toolLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
	grid->setVerticalSpacing(0);
	grid->addWidget(toolWidget, 0, 0, 1, 1);

	textLabel = new QLabel();

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-size: 14px;\n"
	"}\n"
	"QWidget{\n"
		"background: rgb(14,22,33)\n"
	"}"));

	textLabel->setStyleSheet("QLabel{\n"
		"background: transparent;\n"
		"color: white;\n"
	"}");

	textLabel->setAttribute(Qt::WA_NoSystemBackground);
	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignLeft);
	textLabel->setContentsMargins(8, 0, 20, 8);

	QStyleOptionTitleBar option;
	option.initFrom(this);
	qDebug() << _chatScrollArea->verticalScrollBar()->width();
	panelWidth = (screenWidth / 3);
	setFixedSize(panelWidth, screenHeight);
	move(screenWidth - width(), 0);
	setContentsMargins(0, 0, 0, 15);
	grid->setContentsMargins(0, 0, 0, 15);
	grid->setHorizontalSpacing(0);
	setWindowFlag(Qt::SplashScreen);

	QWidget* messageWidget = new QWidget();
	_messageLayout = new QGridLayout(messageWidget);
	messageWidget->setContentsMargins(0, 0, 0, 0);
	_messageLayout->setContentsMargins(0, 0, 0, 0);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
		"border-radius: 10px;\n"
	"}");

	setSource(messageText, attachmentsPaths);

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	messageWidget->setMouseTracking(true);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
	qDebug() << _chatScrollArea->size();
	qDebug() << messageWidget->size();
}

QWidget*  MessageWidget::createMessageWidget() {
	QWidget* messageWidget = new QWidget();
	_messageLayout = new QGridLayout(messageWidget);
	messageWidget->setContentsMargins(0, 0, 0, 0);
	_messageLayout->setContentsMargins(0, 0, 0, 0);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
		"border-radius: 10px;\n"
		"}");

	setSource(messageText, attachmentsPaths);

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	messageWidget->setMouseTracking(true);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void MessageWidget::setSource(const QString& messageText, const QUrlList& attachmentsPaths) {
	foreach(const QUrl& url, attachmentsPaths) {
		QString sourcePath;
		if (url.path().at(0) == "/"[0])
			sourcePath = url.path().remove(0, 1);
		else
			sourcePath = url.path();

		if (MediaPlayer::detectMediaType(url.path()).contains("image")) {
			_messageAttachment = new MessageAttachment(sourcePath);

			QImage image(sourcePath);
			QSize size = getMinimumSizeWithAspectRatio( image.size(), panelWidth - _chatScrollArea->verticalScrollBar()->width());
			image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

			_messageAttachment->setPixmap(QPixmap::fromImage(image));
			_messageAttachment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

			textLabel->setText(messageText);
			_messageLayout->addWidget(_messageAttachment, _messageLayout->rowCount(), 0, 1, 1);
			_messageLayout->addWidget(textLabel, _messageLayout->rowCount(), 0, 1, 1);
			_chatScrollAreaLayout->addWidget(messageWidget, 1, 0, 1, 1, Qt::AlignCenter);

			connect(_messageAttachment, &MessageAttachment::clicked, this, &MessageWidget::attachment_cliked);
		}
	}
}

QSize MessageWidget::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageWidget::attachment_cliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();	

	if (attachment->attachmentType() == "image")
		return;

	_mediaPlayer->setSource(QUrl::fromLocalFile(attachment->attachmentPath()));
	_mediaPlayer->exec();
}