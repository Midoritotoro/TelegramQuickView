#include "MessageWidget.h"

#include <Windows.h>
#include <cmath>
#include <QResizeEvent>
#include <QStyleOptionTitleBar>
#include <QFile>


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
	QGridLayout* chatScrollAreaLayout = new QGridLayout(chatScrollAreaWidget);
	_chatScrollArea = new QScrollArea();
	_chatScrollArea->setWidgetResizable(true);

	chatScrollAreaLayout->setContentsMargins(0, 0, 0, 0);
	chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	_chatScrollArea->setContentsMargins(0, 0, 0, 0);
	chatScrollAreaLayout->setHorizontalSpacing(0);

	chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	_chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

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
		"font-family: centry gothic;\n"
		"font-size: 20px;\n"
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
	textLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

	QStyleOptionTitleBar option;
	option.initFrom(this);

	panelWidth = screenWidth / 3;
	setFixedSize(panelWidth, screenHeight);
	move(screenWidth - width(), 0);
	setContentsMargins(0, 0, 0, 15);
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setHorizontalSpacing(0);
	setWindowFlag(Qt::SplashScreen);

	messageWidget = new QWidget();
	gridLayout = new QGridLayout(messageWidget);
	messageWidget->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
		"border-radius: 10px;\n"
	"}");

	grid->addWidget(_chatScrollArea, 1, 0, 1, 1);
	grid->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	chatScrollAreaLayout->addWidget(messageWidget, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);
	
	gridLayout->setAlignment(Qt::AlignCenter);
	gridLayout->setVerticalSpacing(10);
	
	setSource(messageText, attachmentsPaths);

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	messageWidget->setMouseTracking(true);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
	qDebug() << _chatScrollArea->size();
	qDebug() << chatScrollAreaWidget->size();
	qDebug() << messageWidget->size();
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
			QSize size = getMinimumSizeWithAspectRatio( image.size(), panelWidth);
			image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

			_messageAttachment->setPixmap(QPixmap::fromImage(image));
			_messageAttachment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

			textLabel->setText(messageText);
			gridLayout->addWidget(_messageAttachment, 0, 0, 1, 1);
			gridLayout->addWidget(textLabel, 1, 0, 1, 1);

			connect(_messageAttachment, &MessageAttachment::clicked, this, &MessageWidget::attachment_cliked);
			qDebug() << _messageAttachment->size();
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