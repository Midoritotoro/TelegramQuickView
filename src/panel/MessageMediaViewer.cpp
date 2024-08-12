#include "MessageMediaViewer.h"

#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QStyle>

#include "History.h"
#include "NavigationButton.h"
#include "../media/WidgetsHider.h"
#include <QShortcut>
#include <QKeySequence>


MessageMediaViewer::MessageMediaViewer(History* messagesHistory, QWidget* parent)
: QWidget(parent)
, _messagesHistory(messagesHistory)
{
	setMouseTracking(true);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget{\n "
		"background-color: rgba(35, 36, 37, 60)\n"
		"}");

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	QString arrowPath = cssDir.absolutePath() + "/arrow_right.png";

	_grid = new QGridLayout(this);
	_mediaPlayer = new MediaPlayer();
	_mediaPlayer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_grid->setSpacing(0);
	_grid->setContentsMargins(0, 0, 0, 0);
	setContentsMargins(0, 0, 0, 0);

	_grid->addWidget(_mediaPlayer, _grid->rowCount(), 0, 1, 1);

	QTransform transform;
	transform.rotate(180);

	_nextAttachment = new NavigationButton(this);
	_previousAttachment = new NavigationButton(this);

	_nextAttachment->setFixedSize(50, 50);
	_previousAttachment->setFixedSize(50, 50);

	_nextAttachment->setIcon(QIcon(arrowPath));
	_nextAttachment->setCursor(Qt::PointingHandCursor);

	_previousAttachment->setIcon(QIcon(QPixmap(arrowPath).transformed(transform)));
	_previousAttachment->setCursor(Qt::PointingHandCursor);

	_mediaPlayer->setVisible(false);

	_previousAttachment->setVisible(true);
	_nextAttachment->setVisible(true);

	QShortcut* nextAttachmentShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
	QShortcut* previousAttachmentShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);

	connect(nextAttachmentShortcut, &QShortcut::activated, this, &MessageMediaViewer::nextAttachmentButton_clicked);
	connect(previousAttachmentShortcut, &QShortcut::activated, this, &MessageMediaViewer::previousAttachmentButton_clicked);

	QWidgetList widgetsList = QWidgetList({ _previousAttachment, _nextAttachment });
	WidgetsHider widgetsHider(widgetsList, false);
	widgetsHider.SetInactivityDuration(1500);

	connect(_nextAttachment, &NavigationButton::clicked, this, &MessageMediaViewer::nextAttachmentButton_clicked);
	connect(_previousAttachment, &NavigationButton::clicked, this, &MessageMediaViewer::previousAttachmentButton_clicked);
}

void MessageMediaViewer::updateMediaNavigationButtons() {
	if (nextMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage)) == -1
		&& _currentMessage->attachmentAt(_currentMessageAttachmentIndex + 1) == nullptr
	) // Впереди нет сообщений с медиа
		_nextAttachment->hide();

	if (previousMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage)) == -1
		&& _currentMessage->attachmentAt(_currentMessageAttachmentIndex - 1) == nullptr
	) // Позади нет сообщений с медиа
		_previousAttachment->hide();
}

void MessageMediaViewer::openMessageAttachment(MessageWidget* messageWidget, int triggeredAttachmentIndex) {
	_currentMessage = messageWidget;
	_currentMessageAttachmentIndex = triggeredAttachmentIndex;

	updateMediaNavigationButtons();

	_mediaPlayer->setVisible(true);
	_mediaPlayer->setSource(QUrl::fromLocalFile(messageWidget->attachmentAt(triggeredAttachmentIndex)->attachmentPath()));
	
	showFullScreen();
	_mediaPlayer->showFullScreen();
}

int MessageMediaViewer::nextMessageWithAttachmentsIndex(int currentIndex) const noexcept {
	for (int i = currentIndex + 1; i < _messagesHistory->count(); ++i)
		if (_messagesHistory->messageAt(i)->hasAttachments())
			return i;
	return -1;
}

int MessageMediaViewer::previousMessageWithAttachmentsIndex(int currentIndex) const noexcept {
	for (int i = currentIndex - 1; i >= 0; --i)
		if (_messagesHistory->messageAt(i)->hasAttachments())
			return i;
	return -1;
}

void MessageMediaViewer::goToPreviousMessage() {
	int previousMessageIndex = previousMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage));

	if (previousMessageIndex != -1) {
		if (previousMessageIndex >= 0) {
			MessageWidget* previousMessage = _messagesHistory->messageAt(previousMessageIndex);

			if (previousMessage) {
				_currentMessage = previousMessage;

				_currentMessageAttachmentIndex = _currentMessage->attachmentsLength() - 1;
				_mediaPlayer->setSource(QUrl::fromLocalFile(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath()));

				if (_nextAttachment->isHidden())
					_nextAttachment->show();
			}
		}
	}
}

void MessageMediaViewer::goToNextMessage() {
	int nextMessageIndex = nextMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage));

	if (nextMessageIndex != -1) {
		if (nextMessageIndex >= 0) {
			MessageWidget* nextMessage = _messagesHistory->messageAt(nextMessageIndex);

			if (nextMessage) {
				_currentMessage = nextMessage;
				
				_currentMessageAttachmentIndex = 0;
				_mediaPlayer->setSource(QUrl::fromLocalFile(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath()));

				if (_previousAttachment->isHidden())
					_previousAttachment->show();
			}
		}
	}
}

void MessageMediaViewer::nextAttachmentButton_clicked() {
	int messageAttachmentsCount = _currentMessage->attachmentsLength();

	for (int index = 0; index < messageAttachmentsCount; ++index) {
		if ((messageAttachmentsCount - (_currentMessageAttachmentIndex + 1)) <= 0) {
			// Медиа в текущем сообщении нет, идём к следующему
			goToNextMessage();
			break;
		}

		MessageAttachment* attachment = _currentMessage->attachmentAt(_currentMessageAttachmentIndex);

		if (_currentMessage->indexOfAttachment(attachment) == index) {
			if (messageAttachmentsCount - (index + 1) > 0) {
				_currentMessageAttachmentIndex = index + 1;

				_mediaPlayer->setSource(QUrl::fromLocalFile(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath()));
				qDebug() << _currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath();

				if (_previousAttachment->isHidden())
					_previousAttachment->show();

				break;
			}
		}
	}
	updateMediaNavigationButtons();
}

void MessageMediaViewer::previousAttachmentButton_clicked() {
	int messageAttachmentsCount = _currentMessage->attachmentsLength();

	for (int index = messageAttachmentsCount; index >= 0; --index) {
		if (_currentMessageAttachmentIndex <= 0) {
			goToPreviousMessage();
			// Медиа в текущем сообщении нет, идём к предыдущему
			break;
		}

		MessageAttachment* attachment = _currentMessage->attachmentAt(_currentMessageAttachmentIndex);

		if (_currentMessage->indexOfAttachment(attachment) == index) {
			if (index - 1 >= 0) {
				_currentMessageAttachmentIndex = index - 1;
				
				_mediaPlayer->setSource(QUrl::fromLocalFile(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath()));
				qDebug() << _currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath();

				if (_nextAttachment->isHidden())
					_nextAttachment->show();

				break;
			}
		}
	}
	updateMediaNavigationButtons();
}

void MessageMediaViewer::resizeEvent(QResizeEvent* event) {
	_previousAttachment->move(_nextAttachment->width(), height() / 2);
	_nextAttachment->move(width() - (_previousAttachment->width() * 2), height() / 2);
}
