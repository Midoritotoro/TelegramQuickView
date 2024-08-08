#include "TelegramPostQuickView.h"


#include "../media/WidgetsHider.h"
#include <QScrollBar>


TelegramPostQuickView::TelegramPostQuickView(const QString& messageText, const QUrlList& attachmentsPaths, QWidget* parent) :
	QWidget(parent)
{
	setMouseTracking(true);

	int screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	int screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	panelWidth = screenWidth / 3;
	setFixedSize(panelWidth, screenHeight);
	move(screenWidth - width(), 0);

	_messageMediaViewer = new MessageMediaViewer();

	_chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QGridLayout(_chatScrollAreaWidget);
	_chatScrollArea = new QScrollArea();

	_chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(width() / 25, 0, width() / 3.5, 15);
	_chatScrollAreaLayout->setVerticalSpacing(15);

	_chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	_chatScrollArea->setContentsMargins(0, 0, 0, 0);

	_chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	_chatScrollArea->setWidget(_chatScrollAreaWidget);
	_chatScrollArea->setMouseTracking(true);
	_chatScrollAreaWidget->setMouseTracking(true);

	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString chatScrollAreaStylePath = cssDir.absolutePath() + "/ChatScrollAreaStyle.css";

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	closeWindowButton->setStyleSheet("QToolButton{\n"
		"background-color: transparent;\n"
		"}\n"
	"QToolButton::hover{\n"
		"background-color: rgba(50, 60, 75, 1);\n"
	"}");

	minimizeWindowButton->setStyleSheet("QToolButton{\n"
		"background-color: transparent;\n"
		"}\n"
		"QToolButton::hover{\n"
		"background-color: rgba(50, 60, 75, 1);\n"
		"}");

	QFile chatScrollAreaStyleFile(chatScrollAreaStylePath);

	if (chatScrollAreaStyleFile.open(QFile::ReadOnly)) {

		QByteArray chatScrollAreaStyle = chatScrollAreaStyleFile.readAll();
		_chatScrollArea->setStyleSheet(chatScrollAreaStyle);

		chatScrollAreaStyleFile.close();
	}

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

	setContentsMargins(0, 0, 0, 0);

	_grid->setContentsMargins(0, 0, 3, 4);
	_grid->setVerticalSpacing(4);
	_grid->setHorizontalSpacing(0);
	_grid->addWidget(_chatScrollArea, _grid->rowCount(), 0, 1, 1);

	setWindowFlag(Qt::SplashScreen);

	addMessage(messageText, attachmentsPaths);

	QWidgetList widgetsList = QWidgetList({ _chatScrollArea->verticalScrollBar() });
	WidgetsHider& widgetsHider = WidgetsHider::Instance(widgetsList, true);
	widgetsHider.SetInactivityDuration(1000);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
}

void TelegramPostQuickView::addMessage(const QString& messageText, const QUrlList& attachmentsPaths) {
	const int maximumMessageWidth = panelWidth - (width() / 25 + width() / 3.5);
	MessageWidget* messageWidget = new MessageWidget();

	messageWidget->addMessageAttachments(attachmentsPaths, maximumMessageWidth);
	messageWidget->addMessageText(messageText);

	_chatScrollAreaLayout->addWidget(messageWidget, _chatScrollAreaLayout->rowCount(), 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
	if (messageWidget->getMessageAttachment())
		connect(messageWidget->getMessageAttachment(), &MessageAttachment::clicked, this, &TelegramPostQuickView::attachmentCliked);
}

void TelegramPostQuickView::attachmentCliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();
	_messageMediaViewer->show();
	_messageMediaViewer->openMessageAttachment(attachment);
}