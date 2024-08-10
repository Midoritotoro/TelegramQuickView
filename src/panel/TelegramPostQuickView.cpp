#include "TelegramPostQuickView.h"


#include "../media/WidgetsHider.h"
#include <QScrollBar>


TelegramPostQuickView::TelegramPostQuickView(QWidget* parent) :
	QWidget(parent)
{
	setMouseTracking(true);

	int screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	int screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	_panelWidth = screenWidth / 3;
	setFixedSize(_panelWidth, screenHeight);
	move(screenWidth - width(), 0);

	_messageMediaViewer = new MessageMediaViewer();

	QWidget* chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QGridLayout(chatScrollAreaWidget);
	QScrollArea* chatScrollArea = new QScrollArea();

	chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(width() / 25, 0, width() / 3.5, 15);
	_chatScrollAreaLayout->setVerticalSpacing(15);

	chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	chatScrollArea->setContentsMargins(0, 0, 0, 0);

	chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	chatScrollArea->setWidget(chatScrollAreaWidget);
	chatScrollArea->setMouseTracking(true);
	chatScrollAreaWidget->setMouseTracking(true);

	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString chatScrollAreaStylePath = cssDir.absolutePath() + "/ChatScrollAreaStyle.css";

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

	minimizeWindowButton->setIcon(minPix);
	closeWindowButton->setIcon(closePix);

	closeWindowButton->setStyleSheet("QToolButton{\n"
		"background-color: transparent;\n"
		"}\n"
	"QToolButton::hover{\n"
		"background: rgb(57, 58, 59);\n"
		"color: white;\n"
	"}");

	minimizeWindowButton->setStyleSheet("QToolButton{\n"
		"background-color: transparent;\n"
		"}\n"
		"QToolButton::hover{\n"
		"background: rgb(79, 80, 81);\n"
		"color: white;\n"
		"}");

	QFile chatScrollAreaStyleFile(chatScrollAreaStylePath);

	if (chatScrollAreaStyleFile.open(QFile::ReadOnly)) {

		QByteArray chatScrollAreaStyle = chatScrollAreaStyleFile.readAll();
		chatScrollArea->setStyleSheet(chatScrollAreaStyle);

		chatScrollAreaStyleFile.close();
	}

	QGridLayout* grid = new QGridLayout(this);

	QWidget* toolWidget = new QWidget();
	QGridLayout* toolLayout = new QGridLayout(toolWidget);
	//toolWidget->setObjectName("toolWidget");
	//toolWidget->setStyleSheet("#toolWidget{\n"
	//	"background-color: rgba(35,36,37, 80);\n"
	//	"}");
	toolWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	toolWidget->setContentsMargins(0, 0, 0, 0);
	toolLayout->setContentsMargins(0, 0, 0, 0);
	toolLayout->setSpacing(0);

	toolLayout->addWidget(minimizeWindowButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(closeWindowButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);

	toolLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
	grid->setVerticalSpacing(0);
	//_grid->addWidget(toolWidget, _grid->rowCount(), 0, 1, 1);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-size: 14px;\n"
		"}\n"
		"QWidget{\n"
		"background-color: rgba(35,36,37, 60);\n"
		"}"));

	setContentsMargins(0, 0, 0, 0);

	grid->setContentsMargins(0, 0, 3, 4);
	grid->setVerticalSpacing(4);
	grid->setHorizontalSpacing(0);
	grid->addWidget(chatScrollArea, grid->rowCount(), 0, 1, 1);

	QWidgetList widgetsList = QWidgetList({ chatScrollArea->verticalScrollBar() });
	WidgetsHider& widgetsHider = WidgetsHider::Instance(widgetsList, true);
	widgetsHider.SetInactivityDuration(1000);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
}

void TelegramPostQuickView::makeMessage(const QString& author, const QString& messageText, const QUrlList& attachmentsPaths) {
	const int maximumMessageWidth = _panelWidth - (width() / 25 + width() / 3.5);
	MessageWidget* messageWidget = new MessageWidget(author);

	messageWidget->addMessageAttachments(attachmentsPaths, maximumMessageWidth);
	messageWidget->addMessageText(messageText);

	_chatScrollAreaLayout->addWidget(messageWidget, _chatScrollAreaLayout->rowCount(), 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
	_messagesList.append(messageWidget);

	const MessageAttachmentsList& messageAttachmentsList = messageWidget->messageAttachments();

	if (messageAttachmentsList.isEmpty())
		return;

	foreach(MessageAttachment* messageAttachment, messageAttachmentsList)
		connect(messageAttachment, &MessageAttachment::clicked, this, &TelegramPostQuickView::attachmentCliked);
}

void TelegramPostQuickView::attachmentCliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();
	MessageWidget* attachmentParentMessage = attachment->parentMessage();

	_messageMediaViewer->show();
	_messageMediaViewer->openMessageAttachment(attachmentParentMessage, attachmentParentMessage->indexOfAttachment(attachment));
	showMinimized();
}

const MessagesList& TelegramPostQuickView::messages() const {
	return _messagesList;
}

int TelegramPostQuickView::indexOfMessage(MessageWidget* messageWidget) {
	return _messagesList.indexOf(messageWidget);
}

MessageWidget* TelegramPostQuickView::messagetAt(int index) {
	return _messagesList.at(index);
}