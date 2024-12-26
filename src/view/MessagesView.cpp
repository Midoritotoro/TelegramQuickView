#include "MessagesView.h"

#include "../ui/WidgetsHider.h"
#include "../core/Time.h"

#include "../ui/style/StyleCore.h"
#include "../ui/style/StyleWidgets.h"

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

	auto scrollInner = new InnerWidget();
	_chatScrollArea = new ContinuousScroll(this);

	scrollInner->setMinimumHeight(screenHeight);

	scrollInner->setOpacity(0.5);

	_chatScrollArea->setOpacity(0);
	_chatScrollArea->setTrackingContent(true);

	_messageMediaViewer = std::make_unique<MediaViewer>(_messagesHistory.get());

	scrollInner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_chatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_chatScrollArea->setWidget(scrollInner);

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
}

void MessagesView::makeMessage(
	const QString& messageText, 
	const QStringList& attachmentsPaths)
{
	auto ms = Time::now();
	const auto timer = gsl::finally([&ms] { qDebug() << "TelegramPostQuickView::makeMessage: " << Time::now() - ms << " ms";  });

	auto message = new Message(this);

	message->setMediaDisplayMode(_displayMode);
	message->setAttributes(messageText, attachmentsPaths);

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

void MessagesView::paintEvent(QPaintEvent * event) 
{}

static QString text = QString("https://www.google.com/\n"
	+ QString("MSDN's primary web presence was at msdn.microsoft.com, and was a collection of sites for the developer community that provide information, documentation, and discussion that is authored both by Microsoft and by the community at large. Microsoft had placed emphasis on incorporation of forums, blogs, library annotations, and social bookmarking to make MSDN an open dialog with the developer community rather than a one-way service.[1] The main website, and most of its constituent applications below were available in 56[2] or more languages.\n")
	+ "Library\n"
	+ "The MSDN Library is the centralized repository of official developer - related documentation.Document sets are published by various user assistance organizations within Microsoft, and the community has the opportunity on many of the pages to add their own annotations.Community Content contributions can be edited by anyone.\n"
	+ "MSDN Forums are the web - based forums used by the community to discuss a wide variety of software development topics.MSDN Forums were migrated to an all - new platform during 2008 that provided new features designed to improve efficiency such as inline preview of threads, AJAX filtering, and a slide - up post editor.\n"
	+ "Blogs\n"
	+ "MSDN blogs is a series of blogs that were hosted under Microsoft's domain blogs.msdn.com. Some blogs are dedicated to a product (e.g. Visual Studio, Internet Explorer, PowerShell) or a version of a product (e.g Windows 7, Windows 8), while others belong to a Microsoft employee (e.g. Michael Howard or Raymond Chen.\n"
	+ "In May 2020, the MSDN and TechNet blogs were closed and the content was archived at Microsoft Docs.\n");

static QString videoPath = "C:\\Users\\danya\\Downloads\\videotestvertical.mp4";
static QString photoPath = "C:\\Users\\danya\\Downloads\\code.png";


void MessagesView::addContent() {
	const auto guard = gsl::finally([this] { _chatScrollArea->disableScroll(false); });
	_chatScrollArea->disableScroll(true);

	makeMessage(text, { videoPath, videoPath, videoPath, videoPath, videoPath });

	//for (int count = 0; count < 5; ++count) {
	//	//const auto message = _telegramParser->loadMessage();

	//	if (message.isNull())
	//		continue;

	//	message.attachments.isEmpty()
	//		? makeMessage(message.text)
	//		: makeMessage(message.text, message.attachments);
	//}
}
