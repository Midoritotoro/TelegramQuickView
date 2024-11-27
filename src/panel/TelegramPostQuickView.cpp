#include "TelegramPostQuickView.h"

#include "../media/player/WidgetsHider.h"
#include "../core/StyleCore.h"

#include "MessageAttachment.h"
#include "MessageMediaViewer.h"

#include "History.h"

#include <QShowEvent>
#include <QElapsedTimer>
#include <QScrollBar>
#include <QDir>


TelegramPostQuickView::TelegramPostQuickView(QWidget* parent):
	QWidget(parent)
	, _displayMode(MessageWidget::MessageMediaDisplayMode::PreviewWithCount)
	, _sqlReader(std::make_unique<SqlReader>())
	, _currentPostIndex(1)
{
	setMouseTracking(true);

	_messagesHistory = std::make_shared<History>();

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	const auto panelWidth = (screenWidth / 3.5);

	style::maximumMessageWidth = panelWidth - (width() / 12.5);

	resize(panelWidth, screenHeight);
	move(screenWidth - width(), 0);

	QWidget* chatScrollAreaWidget = new QWidget();
	_chatScrollAreaLayout = new QVBoxLayout(chatScrollAreaWidget);
	_chatScrollArea = new ContinuousScroll(this);
	
	_chatScrollArea->setOpacity(0.1);
	_chatScrollArea->setTrackingContent(true);

	_messageMediaViewer = std::make_unique<MessageMediaViewer>(_messagesHistory.get());

	_chatScrollArea->setWidgetResizable(true);

	_chatScrollAreaLayout->setContentsMargins(width() / 25, 0, width() / 25, 15);
	_chatScrollAreaLayout->setSpacing(15);

	chatScrollAreaWidget->setContentsMargins(0, 0, 0, 0);
	_chatScrollArea->setContentsMargins(0, 0, 0, 0);

	chatScrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setWidget(chatScrollAreaWidget);

	chatScrollAreaWidget->setMouseTracking(true);

	QGridLayout* grid = new QGridLayout(this);

	grid->setVerticalSpacing(0);

	grid->setContentsMargins(0, 0, 5, 4);
	grid->setVerticalSpacing(4);
	grid->setHorizontalSpacing(0);

	grid->addWidget(_chatScrollArea, grid->rowCount(), 0, 1, 1);

	QWidgetList widgetsList = QWidgetList({ _chatScrollArea->verticalScrollBar() });
	WidgetsHider* widgetsHider = new WidgetsHider(true, true, widgetsList);

	setStyleSheet(QString::fromUtf8("QWidget {\n"
		"background-color: rgba(0, 0, 0, 0); }\n"
	));

	setWindowFlag(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	setContentsMargins(0, 0, 0, 0);

	widgetsHider->SetInactivityDuration(1500);
	widgetsHider->SetAnimationDuration(1500);

	connect(_messageMediaViewer.get(), &MessageMediaViewer::escaped, this, &TelegramPostQuickView::showNormal);
	connect(_messageMediaViewer.get(), &MessageMediaViewer::needScrollToMessage, _chatScrollArea, &ScrollArea::scrollToWidget);

	connect(_chatScrollArea, &ContinuousScroll::addContentRequest, this, &TelegramPostQuickView::addContent);
}

void TelegramPostQuickView::makeMessage(const QString& messageText, const QStringList& attachmentsPaths) {
	auto ms = Time::now();
	const auto timer = Guard::finally([&ms] { qDebug() << "TelegramPostQuickView::makeMessage: " << Time::now() - ms << " ms";  });

	auto message = new MessageWidget();

	message->setMessageMediaDisplayMode(_displayMode);
	message->addMessageAttachments(attachmentsPaths);
	message->addMessageText(messageText);

	_chatScrollAreaLayout->addWidget(message, 0, Qt::AlignLeft | Qt::AlignTop);
	_messagesHistory->makeMessage(message);

	if (!message->hasAttachments())
		return;

	const auto& messageAttachmentsList = message->messageAttachments();

	foreach(auto attachment, messageAttachmentsList)
		connect(attachment, &MessageAttachment::clicked, this, &TelegramPostQuickView::attachmentCliked);
}

void TelegramPostQuickView::setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode displayMode) {
	_displayMode = displayMode;
}

void TelegramPostQuickView::attachmentCliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();

	if (_messageMediaViewer == nullptr)
		_messageMediaViewer = std::make_unique<MessageMediaViewer>(_messagesHistory.get());

	_messageMediaViewer->openMessageAttachment(attachment->parentMessage(), attachment->parentMessage()->indexOfAttachment(attachment));
	_messageMediaViewer->show();

	showMinimized();
}

void TelegramPostQuickView::addContent() {
	const auto message = _sqlReader->getMessage(_currentPostIndex);
	++_currentPostIndex;

	if (message.isNull()) 
		return;

	if (message.attachments.isEmpty() == false)
		makeMessage(message.text, message.attachments);
	else
		makeMessage(message.text);
}
