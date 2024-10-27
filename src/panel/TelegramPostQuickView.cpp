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

	setStyleSheet(QString::fromUtf8("*{\n"
		"font-size: 14px;\n"
		"}\n"
		"QWidget{\n"
		"background-color: rgba(35,36,37, 60);\n"
		"}"));

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
	connect(_chatScrollArea, &ContinuousScroll::addContentRequest, this, &TelegramPostQuickView::addContentsRequest);
}

void TelegramPostQuickView::makeMessage(const QString& messageText, const QUrlList& attachmentsPaths) {
	QElapsedTimer timer;
	timer.start();

	const auto maximumMessageWidth = _panelWidth - (width() / 12.5);
	MessageWidget* messageWidget = new MessageWidget();

	messageWidget->setMessageMediaDisplayMode(_displayMode);
	messageWidget->addMessageAttachments(attachmentsPaths, maximumMessageWidth);
	messageWidget->addMessageText(messageText);

	_chatScrollAreaLayout->addWidget(messageWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
	_messagesHistory->makeMessage(messageWidget);

	if (!messageWidget->hasAttachments())
		return;

	const auto& messageAttachmentsList = messageWidget->messageAttachments();

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

void TelegramPostQuickView::addContentsRequest() {

	makeMessage("text of message.", QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test8.jpg") });
}
