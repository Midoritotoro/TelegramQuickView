#include "TelegramPostQuickView.h"


#include "../media/WidgetsHider.h"
#include <QScrollBar>
#include <QShowEvent>


TelegramPostQuickView::TelegramPostQuickView(QWidget* parent):
QWidget(parent)
, _displayMode(MessageMediaDisplayMode::Stack)
{
	setMouseTracking(true);
	_messagesHistory = new History();

	const int screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const int screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	_panelWidth = screenWidth / 3.5;
	setFixedSize(_panelWidth, screenHeight);
	move(screenWidth - width(), 0);

	_messageMediaViewer = new MessageMediaViewer(_messagesHistory);

	QWidget* chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QVBoxLayout(chatScrollAreaWidget);
	_chatScrollArea = new ScrollArea();

	_chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(width() / 25, 0, width() / 25, 15);
	_chatScrollAreaLayout->setSpacing(15);

	chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	_chatScrollArea->setContentsMargins(0, 0, 0, 0);

	chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	_chatScrollArea->setWidget(chatScrollAreaWidget);
	_chatScrollArea->setMouseTracking(true);
	chatScrollAreaWidget->setMouseTracking(true);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../src/css");

	QString chatScrollAreaStylePath = cssDir.absolutePath() + "/ChatScrollAreaStyle.css";
	QFile chatScrollAreaStyleFile(chatScrollAreaStylePath);
	if (chatScrollAreaStyleFile.open(QFile::ReadOnly)) {

		QByteArray chatScrollAreaStyle = chatScrollAreaStyleFile.readAll();
		_chatScrollArea->setStyleSheet(chatScrollAreaStyle);

		chatScrollAreaStyleFile.close();
	}

	QGridLayout* grid = new QGridLayout(this);

	grid->setVerticalSpacing(0);

	setWindowFlag(Qt::FramelessWindowHint);
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
	grid->addWidget(_chatScrollArea, grid->rowCount(), 0, 1, 1);

	QWidgetList widgetsList = QWidgetList({ _chatScrollArea->verticalScrollBar() });
	WidgetsHider* widgetsHider = new WidgetsHider(widgetsList, true);
	widgetsHider->SetInactivityDuration(1500);
	widgetsHider->SetAnimationDuration(1500);

	connect(_messageMediaViewer, &MessageMediaViewer::escaped, this, &TelegramPostQuickView::showNormal);
	connect(_chatScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, _chatScrollArea, &ScrollArea::scrollHandler);
	connect(_chatScrollArea, &ScrollArea::addContentsNeeded, this, &TelegramPostQuickView::addContentsRequest);
}

void TelegramPostQuickView::makeMessage(const QString& author, const QString& messageText, const QUrlList& attachmentsPaths) {
	const int maximumMessageWidth = _panelWidth - (width() / 12.5);
	MessageWidget* messageWidget = new MessageWidget(author);

	messageWidget->addMessageAttachments(attachmentsPaths, maximumMessageWidth);
	messageWidget->addMessageText(messageText);

	switch (_displayMode) {
	case MessageMediaDisplayMode::PreviewWithCount:
	case MessageMediaDisplayMode::Compact:
		default:
			_chatScrollAreaLayout->addWidget(messageWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
	}

	_messagesList.append(messageWidget);

	const MessageAttachmentsList& messageAttachmentsList = messageWidget->messageAttachments();
	_messagesHistory->makeMessage(messageWidget);

	if (messageAttachmentsList.isEmpty())
		return;

	foreach(MessageAttachment* messageAttachment, messageAttachmentsList)
		connect(messageAttachment, &MessageAttachment::clicked, this, &TelegramPostQuickView::attachmentCliked);
}

void TelegramPostQuickView::setMessageMediaDisplayMode(MessageMediaDisplayMode displayMode) {
	_displayMode = displayMode;
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

MessageWidget* TelegramPostQuickView::messageAt(int index) {
	return _messagesList.at(index);
}

void TelegramPostQuickView::showEvent(QShowEvent* event) {
	_chatScrollArea->verticalScrollBar()->setValue(_chatScrollArea->verticalScrollBar()->minimum());
	QWidget::showEvent(event);
}

void TelegramPostQuickView::addContentsRequest() {
	// Запарсить файл с постами и подгрузить более старые
	makeMessage("Username1", "text", QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4") });
}