#include "MessageWidget.h"

#include <Windows.h>
#include <cmath>
#include <QResizeEvent>
#include <QStyleOptionTitleBar>


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

	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();
	QToolButton* maximizeWindowButton = new QToolButton();

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	QPixmap maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);
	maximizeWindowButton->setIcon(maxPix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	maximizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	//minimizeWindowButton->setFixedSize(panelWidth / 60, screenHeight / 40);
	//closeWindowButton->setFixedSize(panelWidth / 60, screenHeight / 40);
	//maximizeWindowButton->setFixedSize(panelWidth / 60, screenHeight / 40);

	minimizeWindowButton->setStyleSheet("background-color: transparent;");
	closeWindowButton->setStyleSheet("background-color: transparent;");
	maximizeWindowButton->setStyleSheet("background-color: transparent;");


	_mediaPlayer = new MediaPlayer();
	QGridLayout* grid = new QGridLayout(this);
	grid->addWidget(minimizeWindowButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	grid->addWidget(maximizeWindowButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);
	grid->addWidget(closeWindowButton, 0, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);

	textLabel = new QLabel();

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-family: centry gothic;\n"
		"font-size: 20px;\n"
		//"color: rgba(100, 0, 0, 0)\n"
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
	setContentsMargins(0, 0, 0, 0);
	setWindowFlag(Qt::SplashScreen);
	//setAttribute(Qt::WA_TranslucentBackground);

	messageWidget = new QWidget();
	gridLayout = new QGridLayout(messageWidget);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: darkblue;\n"
		"border: 5px;\n"
	"}");

	grid->addWidget(messageWidget, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
	grid->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	_messageAttachment = new MessageAttachment(attachmentsPaths.at(0).path());
	
	gridLayout->setAlignment(Qt::AlignCenter);
	gridLayout->setVerticalSpacing(0);
	
	setSource(messageText, attachmentsPaths);

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	messageWidget->setMouseTracking(true);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
	connect(maximizeWindowButton, &QToolButton::clicked, this, [this]() {
		isFullScreen() ? showNormal() : showFullScreen();
		});
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

			textLabel->setText(messageText);
			gridLayout->addWidget(_messageAttachment, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
			gridLayout->addWidget(textLabel, 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);

			connect(_messageAttachment, &MessageAttachment::clicked, this, &MessageWidget::attachment_cliked);
		}
	}
}

QSize MessageWidget::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageWidget::resizeEvent(QResizeEvent* event) {

	//if (image.width() == 0 || image.height() == 0)
	//	return;

	//QSize size = getMinimumSizeWithAspectRatio( image.size(), panelWidth);
	//image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//_messageAttachment->setPixmap(QPixmap::fromImage(image));

	event->accept();
}

void MessageWidget::attachment_cliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();	

	if (attachment->attachmentType() == "image")
		return;

	_mediaPlayer->setSource(QUrl::fromLocalFile(attachment->attachmentPath()));
	_mediaPlayer->exec();
}