#include "MessageWidget.h"

#include <Windows.h>
#include <cmath>
#include <QResizeEvent>
#include <QStyleOptionTitleBar>
#include <QFile>
#include <QScrollBar>


MessageAttachment::MessageAttachment(QString attachmentPath, int attachmentWidth, QWidget* parent) :
	ClickableLabel(parent), _attachmentPath(attachmentPath), _attachmentType(MediaPlayer::detectMediaType(attachmentPath))
{
	setMouseTracking(true);
	setScaledContents(true);
	setBackgroundRole(QPalette::Dark);
	setCursor(Qt::PointingHandCursor);

	if (_attachmentType.contains("image")) {
		QImage attachmentImage(attachmentPath);
		attachmentImage = attachmentImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		QSize size = getMinimumSizeWithAspectRatio(attachmentImage.size(), attachmentWidth);
		attachmentImage = attachmentImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		setPixmap(QPixmap::fromImage(attachmentImage));
	}
	else if (_attachmentType.contains("video")) {
		QImage attachmentVideoPreview = MediaPlayer::videoPreview(attachmentPath);
		qDebug() << attachmentVideoPreview.size();
		attachmentVideoPreview = attachmentVideoPreview.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		QSize size = getMinimumSizeWithAspectRatio(attachmentVideoPreview.size(), attachmentWidth);
		attachmentVideoPreview = attachmentVideoPreview.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		setPixmap(QPixmap::fromImage(attachmentVideoPreview));
	}
}

QSize MessageAttachment::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
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

	 _chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QVBoxLayout(_chatScrollAreaWidget);
	_chatScrollArea = new QScrollArea();
	_chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(10, 0, 10 + _chatScrollArea->verticalScrollBar()->width(), 15);
	_chatScrollAreaLayout->setSpacing(15);

	_chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	_chatScrollArea->setContentsMargins(0, 0, 0, 0);

	_chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	_chatScrollArea->setWidget(_chatScrollAreaWidget);

	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString toolButtonStylePath = cssDir.absolutePath() + "/ToolButtonStyle.css";
	QString chatScrollAreaStylePath = cssDir.absolutePath() + "/ChatScrollAreaStyle.css";

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	QFile toolButtonStyleFile(toolButtonStylePath);
	QFile chatScrollAreaStyleFile(chatScrollAreaStylePath);
	if (toolButtonStyleFile.open(QFile::ReadOnly) && chatScrollAreaStyleFile.open(QFile::ReadOnly)) {

		QByteArray toolButtonStyle = toolButtonStyleFile.readAll();
		QByteArray chatScrollAreaStyle = chatScrollAreaStyleFile.readAll();

		closeWindowButton->setStyleSheet(toolButtonStyle);
		minimizeWindowButton->setStyleSheet(toolButtonStyle);

		_chatScrollArea->setStyleSheet(chatScrollAreaStyle);

		toolButtonStyleFile.close();
		chatScrollAreaStyleFile.close();
	}

	_mediaPlayer = new MediaPlayer();
	_grid = new QGridLayout(this);

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
	_grid->setVerticalSpacing(0);
	_grid->addWidget(toolWidget, _grid->rowCount(), 0, 1, 1);

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-size: 14px;\n"
	"}\n"
	"QWidget{\n"
		"background: rgb(14,22,33)\n"
	"}"));

	panelWidth = (screenWidth / 3);
	setFixedSize(panelWidth, screenHeight);
	move(screenWidth - width(), 0);

	setContentsMargins(0, 0, 0, 0);

	_grid->setContentsMargins(0, 0, 3, 0);
	_grid->setVerticalSpacing(10);
	_grid->setHorizontalSpacing(0);
	_grid->addWidget(_chatScrollArea, _grid->rowCount(), 0, 1, 1);

	setWindowFlag(Qt::SplashScreen);
	
	setSource(messageText, attachmentsPaths);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
}

QWidget* MessageWidget::createMessageWidget() {
	QWidget* messageWidget = new QWidget();
	QGridLayout* messageLayout = new QGridLayout(messageWidget);
	messageWidget->setContentsMargins(0, 0, 0, 0);
	messageLayout->setContentsMargins(0, 0, 0, 0);

	messageWidget->setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
		"border-radius: 10px;\n"
	"}");

	messageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	messageWidget->setMouseTracking(true);

	return messageWidget;
}

QLabel* MessageWidget::createMessageTextLabel() {
	QLabel* textLabel = new QLabel();

	textLabel->setStyleSheet("QLabel{\n"
		"background: transparent;\n"
		"color: white;\n"
	"}");

	textLabel->setAttribute(Qt::WA_NoSystemBackground);
	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignLeft);
	textLabel->setContentsMargins(8, 0, 20, 8);

	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	return textLabel;
}

void MessageWidget::reorderMessages() {
	for (int index = 0; index < _chatScrollAreaLayout->count(); index++) {
		QWidget* reorderedWidget = _chatScrollAreaLayout->itemAt(index);
		_chatScrollAreaLayout->removeWidget()
	}
}

void MessageWidget::setSource(const QString& messageText, const QUrlList& attachmentsPaths) {
	QWidget* messageWidget = createMessageWidget();
	QGridLayout* messageLayout = qobject_cast<QGridLayout*>(messageWidget->layout());

	foreach(const QUrl& url, attachmentsPaths) {

		QString sourcePath;
		if (url.path().at(0) == "/"[0])
			sourcePath = url.path().remove(0, 1);
		else
			sourcePath = url.path();

		MessageAttachment* messageAttachment = new MessageAttachment(sourcePath, panelWidth - _chatScrollArea->verticalScrollBar()->width());

		messageLayout->addWidget(messageAttachment, messageLayout->rowCount(), 0, 1, 1);
		connect(messageAttachment, &MessageAttachment::clicked, this, &MessageWidget::attachment_cliked);
	}

	if (messageText.length() > 0) {
		QLabel* textLabel = createMessageTextLabel();
		textLabel->setText(messageText);
		messageLayout->addWidget(textLabel, messageLayout->rowCount(), 0, 1, 1);
	}

	_chatScrollAreaLayout->addWidget(messageWidget, 0, Qt::AlignHCenter | Qt::AlignBottom);
}

void MessageWidget::attachment_cliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();	

	if (attachment->attachmentType() == "image")
		return;

	_mediaPlayer->setSource(QUrl::fromLocalFile(attachment->attachmentPath()));
	_mediaPlayer->exec();
}