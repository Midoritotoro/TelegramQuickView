#include "TelegramPostQuickView.h"

#include "../media/player/WidgetsHider.h"

#include "../core/StyleCore.h"
#include "../core/Time.h"

#include "../core/MouseDetector.h"
#include "../parser/TelegramParser.h"

#include "MessageAttachment.h"
#include "History.h"

#include "MessageMediaViewer.h"

#include <QShowEvent>
#include <QElapsedTimer>
#include <QScrollBar>
#include <QDir>


MessagesView::MessagesView(QWidget* parent):
	QWidget(parent)
	, _telegramParser(std::make_unique<TelegramParser>())
//	, _mouseDetector(std::make_unique<MouseDetector>())
	, _displayMode(MessageWidget::MessageMediaDisplayMode::PreviewWithCount)
	, _currentPostIndex(1)
{
	setMouseTracking(true);

	_messagesHistory = std::make_shared<History>();

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	const auto panelWidth = (screenWidth / 3.5);

	style::maximumMessageWidth = panelWidth - (width() / 12.5);
	style::minimumMessageWidth = (panelWidth - (width() / 12.5)) / 10.;

	style::maximumTextWidth = style::maximumMessageWidth;
	style::maximumTextHeight = screenHeight * 2;

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

	//connect(_mouseDetector.get(), &MouseDetector::needsToShow, this, &TelegramPostQuickView::show);
	connect(_chatScrollArea, &ContinuousScroll::addContentRequest, this, &TelegramPostQuickView::addContent);
	//addContent();
	//addContent();
	//addContent();
}

void MessagesView::makeMessage(
	const QString& messageText, 
	const QStringList& attachmentsPaths)
{
	auto ms = Time::now();
	const auto timer = gsl::finally([&ms] { qDebug() << "TelegramPostQuickView::makeMessage: " << Time::now() - ms << " ms";  });

	auto message = new Message();

	message->setMediaDisplayMode(_displayMode);
	message->setAttachments(attachmentsPaths);

	message->setText(messageText);

	_chatScrollAreaLayout->addWidget(message, 0, Qt::AlignLeft | Qt::AlignTop);
	_messagesHistory->makeMessage(message);

	if (!message->hasAttachments())
		return;

	const auto& messageAttachmentsList = message->messageAttachments();

	foreach(auto attachment, messageAttachmentsList)
		connect(attachment, &QAbstractButton::clicked, this, &TelegramPostQuickView::attachmentCliked);
}

void MessagesView::setMessageMediaDisplayMode(Message::MediaDisplayMode displayMode) {
	_displayMode = displayMode;
}

void MessagesView::attachmentCliked() {
	auto attachment = (MessageAttachment*)sender();

	if (_messageMediaViewer == nullptr)
		_messageMediaViewer = std::make_unique<MessageMediaViewer>(_messagesHistory.get());

	_messageMediaViewer->openMessageAttachment(attachment->parentMessage(), attachment->parentMessage()->indexOfAttachment(attachment));
	_messageMediaViewer->show();

	showMinimized();
}

void MessagesView::addContent() {
	const auto guard = gsl::finally([this] { _chatScrollArea->disableScroll(false); });
	_chatScrollArea->disableScroll(true);

	for (int count = 0; count < 5; ++count) {
		const auto message = _telegramParser->loadMessage();

		if (message.isNull())
			continue;

		message.attachments.isEmpty()
			? makeMessage(message.text)
			: makeMessage(message.text, message.attachments);
	}
}
