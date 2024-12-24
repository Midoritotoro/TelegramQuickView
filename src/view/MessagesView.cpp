#include "MessagesView.h"

#include "../ui/WidgetsHider.h"

#include "../ui/style/StyleCore.h"
#include "../core/Time.h"

#include "MessageAttachment.h"
#include "History.h"

#include <QShowEvent>
#include <QElapsedTimer>

#include <QScrollBar>
#include <QDir>

#include <QPainter>


MessagesView::MessagesView(QWidget* parent):
	QWidget(parent)
	//, _telegramParser(std::make_unique<TelegramParser>())
	, _messagesHistory(std::make_shared<History>())
//	, _mouseDetector(std::make_unique<MouseDetector>())
	, _displayMode(Message::MediaDisplayMode::PreviewWithCount)
	, _currentPostIndex(1)
{
	setMouseTracking(true);
	setContentsMargins(0, 0, 0, 0);

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	resize(style::panelWidth, screenHeight);
	move(screenWidth - width(), 0);

	auto scrollInner = new InnerWidget(this);
	_chatScrollArea = new ContinuousScroll(this);

	scrollInner->setOpacity(0.5);

	_chatScrollArea->setOpacity(0);
	_chatScrollArea->setTrackingContent(true);

	_messageMediaViewer = std::make_unique<MediaViewer>(_messagesHistory.get());

	scrollInner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setWidget(scrollInner);
	_chatScrollArea->setWidgetResizable(true);

	scrollInner->setMouseTracking(true);

	auto grid = new QGridLayout(this);

	grid->setContentsMargins(0, 0, 0, 0);

	grid->setVerticalSpacing(0);
	grid->setHorizontalSpacing(0);

	grid->addWidget(_chatScrollArea, grid->rowCount(), 0);

	auto widgetsList = QWidgetList({ _chatScrollArea->verticalScrollBar() });
	auto widgetsHider = new WidgetsHider(true, true, widgetsList);

	setWindowFlag(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	widgetsHider->SetInactivityDuration(1500);
	widgetsHider->SetAnimationDuration(1500);

	connect(_messageMediaViewer.get(), &MediaViewer::escaped, this, &MessagesView::showNormal);
	connect(_messageMediaViewer.get(), &MediaViewer::needScrollToMessage, _chatScrollArea, &ScrollArea::scrollToWidget);

	//connect(_mouseDetector.get(), &MouseDetector::needsToShow, this, &MessagesView::show);
	connect(_chatScrollArea, &ContinuousScroll::addContentRequest, this, &MessagesView::addContent);
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

	auto message = new Message(this);

	message->setMediaDisplayMode(_displayMode);
	message->setAttachments(attachmentsPaths);

	message->setText(messageText);

	_chatScrollArea->addItem(message);
	_messagesHistory->makeMessage(message);
	
	if (!message->hasAttachments())
		return;

	const auto& messageAttachmentsList = message->attachments();

	foreach(auto attachment, messageAttachmentsList)
		connect(attachment, &QAbstractButton::clicked, this, &MessagesView::attachmentCliked);
}

void MessagesView::setMessageMediaDisplayMode(Message::MediaDisplayMode displayMode) {
	_displayMode = displayMode;
}

void MessagesView::attachmentCliked() {
	auto attachment = (MessageAttachment*)sender();

	if (_messageMediaViewer == nullptr)
		_messageMediaViewer = std::make_unique<MediaViewer>(_messagesHistory.get());

	_messageMediaViewer->openMessageAttachment(attachment->parentMessage(), attachment->parentMessage()->indexOfAttachment(attachment));
	_messageMediaViewer->show();

	showMinimized();
}

void MessagesView::paintEvent(QPaintEvent * event) {
	auto painter = QPainter(this);

	painter.setOpacity(0.5);
	painter.fillRect(rect(), Qt::black);

	painter.setRenderHint(QPainter::Antialiasing);

	painter.setPen(Qt::white);
	painter.drawRect(rect().adjusted(0, 0, -1, -1));
}


void MessagesView::addContent() {
	const auto guard = gsl::finally([this] { _chatScrollArea->disableScroll(false); });
	_chatScrollArea->disableScroll(true);

	//for (int count = 0; count < 5; ++count) {
	//	//const auto message = _telegramParser->loadMessage();

	//	if (message.isNull())
	//		continue;

	//	message.attachments.isEmpty()
	//		? makeMessage(message.text)
	//		: makeMessage(message.text, message.attachments);
	//}
}
