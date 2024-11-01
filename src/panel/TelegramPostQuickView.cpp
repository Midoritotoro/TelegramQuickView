#include "TelegramPostQuickView.h"

#include "../media/WidgetsHider.h"
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

	_panelWidth = screenWidth / 3.5;
	resize(_panelWidth, screenHeight);
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

	setStyleSheet(QString::fromUtf8("QWidget { background-color: rgba(0, 0, 0, 0); }\n"
		"*{\n"
		"font-size: 14px;\n"
		"}\n"));

	setContentsMargins(0, 0, 0, 0);

	grid->setContentsMargins(0, 0, 5, 4);
	grid->setVerticalSpacing(4);
	grid->setHorizontalSpacing(0);
	grid->addWidget(_chatScrollArea, grid->rowCount(), 0, 1, 1);

	QWidgetList widgetsList = QWidgetList({ _chatScrollArea->verticalScrollBar() });
	WidgetsHider* widgetsHider = new WidgetsHider(true, widgetsList);
	widgetsHider->SetInactivityDuration(1500);
	widgetsHider->SetAnimationDuration(1500);


	connect(_messageMediaViewer.get(), &MessageMediaViewer::escaped, this, &TelegramPostQuickView::showNormal);
	connect(_chatScrollArea, &ContinuousScroll::addContentRequest, this, &TelegramPostQuickView::addContent);
}

void TelegramPostQuickView::makeMessage(const QString& messageText, const QStringList& attachmentsPaths) {
	QElapsedTimer timer;
	timer.start();

	const auto maximumMessageWidth = _panelWidth - (width() / 12.5);
	MessageWidget* message = new MessageWidget();

	message->setMessageMediaDisplayMode(_displayMode);
	message->addMessageAttachments(attachmentsPaths, maximumMessageWidth);
	message->addMessageText(messageText);

	_chatScrollAreaLayout->addWidget(message, 0, Qt::AlignLeft | Qt::AlignTop);
	_messagesHistory->makeMessage(message);

	if (!message->hasAttachments())
		return;

	const auto& messageAttachmentsList = message->messageAttachments();

	foreach(auto attachment, messageAttachmentsList)
		connect(attachment, &MessageAttachment::clicked, this, &TelegramPostQuickView::attachmentCliked);

	qDebug() << "TelegramPostQuickView::makeMessage: " << static_cast<double>(timer.elapsed()) / 1000 << " s";
}

void TelegramPostQuickView::setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode displayMode) {
	_displayMode = displayMode;
}

void TelegramPostQuickView::attachmentCliked() {
	MessageAttachment* attachment = (MessageAttachment*)sender();

	_messageMediaViewer->openMessageAttachment(attachment->parentMessage(), attachment->parentMessage()->indexOfAttachment(attachment));
	_messageMediaViewer->show();

	showMinimized();
}

void TelegramPostQuickView::showEvent(QShowEvent* event) {
	_chatScrollArea->verticalScrollBar()->setValue(_chatScrollArea->verticalScrollBar()->minimum());
	QWidget::showEvent(event);
}

void TelegramPostQuickView::addContent() {
	const auto message = _sqlReader->getMessage(_currentPostIndex);
	++_currentPostIndex;

	if (message.isNull()) 
		return;

	//qDebug() << message.date << message.attachments << message.text;
	//qDebug() << message.isNull();
	//qDebug() << "attachments.isEmpty(): " << message.attachments.isEmpty();
	//qDebug() << "text.isEmpty(): " << message.text.isEmpty();

	if (message.attachments.isEmpty() == false)
		makeMessage(message.text, message.attachments);
	else
		makeMessage(message.text);
}
