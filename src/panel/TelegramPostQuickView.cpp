#include "TelegramPostQuickView.h"

#include <Windows.h>
#include <QScrollBar>

TelegramPostQuickView::TelegramPostQuickView(const QString& messageText, const QUrlList& attachmentsPaths, QWidget* parent) :
	QWidget(parent)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	_chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QGridLayout(_chatScrollAreaWidget);
	_chatScrollArea = new QScrollArea();
	_chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(10, 0, 10 + _chatScrollArea->verticalScrollBar()->width(), 15);
	_chatScrollAreaLayout->setVerticalSpacing(15);

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

	addMessage(messageText, attachmentsPaths);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
}

void TelegramPostQuickView::addMessage(const QString& messageText, const QUrlList& attachmentsPaths) {
	QWidget* messageWidget = MessageWidget::createMessageWidget();
	QGridLayout* messageLayout = qobject_cast<QGridLayout*>(messageWidget->layout());

	foreach(const QUrl & url, attachmentsPaths) {

		QString sourcePath;
		if (url.path().at(0) == "/"[0])
			sourcePath = url.path().remove(0, 1);
		else
			sourcePath = url.path();

		MessageAttachment* messageAttachment = new MessageAttachment(sourcePath, panelWidth - _chatScrollArea->verticalScrollBar()->width());

		messageLayout->addWidget(messageAttachment, messageLayout->rowCount(), 0, 1, 1);
		connect(messageAttachment, &MessageAttachment::clicked, this, &TelegramPostQuickView::attachment_cliked);
	}

	if (messageText.length() > 0) {
		QLabel* textLabel = MessageWidget::createMessageTextLabel();
		textLabel->setText(messageText);
		messageLayout->addWidget(textLabel, messageLayout->rowCount(), 0, 1, 1);
	}

	_chatScrollAreaLayout->addWidget(messageWidget, _chatScrollAreaLayout->rowCount(), 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom);
}

void TelegramPostQuickView::attachment_cliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();

	if (attachment->attachmentType() == "image")
		return;

	_mediaPlayer->setSource(QUrl::fromLocalFile(attachment->attachmentPath()));
	_mediaPlayer->exec();
}