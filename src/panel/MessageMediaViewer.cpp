#include "MessageMediaViewer.h"

#include "History.h"
#include "NavigationButton.h"
#include "../media/WidgetsHider.h"
#include "MessageTextView.h"
#include "MessageAttachment.h"

#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QStyle>
#include <QShortcut>
#include <QKeySequence>
#include <QPoint>
#include <utility>
#include <QApplication>


namespace {
	constexpr int messageTextViewBottomIndent = 5;
}


MessageMediaViewer::MessageMediaViewer(History* messagesHistory, QWidget* parent)
: QWidget(parent)
, _messagesHistory(messagesHistory)
{
	setMouseTracking(true);

	const int screenHeight = QApplication::primaryScreen()->availableGeometry().width();
	const int screenWidth = QApplication::primaryScreen()->availableGeometry().height();

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget{\n "
		"background-color: rgba(15, 15, 15, 99)\n"
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
	_messageTextView = new MessageTextView(this);

	_nextAttachment->setFixedSize(50, 50);
	_previousAttachment->setFixedSize(50, 50);
	
	_nextAttachment->setIcon(QIcon(arrowPath));
	_nextAttachment->setCursor(Qt::PointingHandCursor);

	_previousAttachment->setIcon(QIcon(QPixmap(arrowPath).transformed(transform)));
	_previousAttachment->setCursor(Qt::PointingHandCursor);

	_mediaPlayer->setVisible(false);

	_previousAttachment->setVisible(true);
	_nextAttachment->setVisible(true);
	_messageTextView->setVisible(true);
	_messageTextView->setMaximumWidth(screenWidth / 2);

	QWidgetList widgetsList = QWidgetList({ _previousAttachment, _nextAttachment, _messageTextView });
	WidgetsHider* widgetsHider = new WidgetsHider(widgetsList, true);
	widgetsHider->SetInactivityDuration(3000);
	widgetsHider->SetAnimationDuration(3000);

	QShortcut* nextAttachmentShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
	QShortcut* previousAttachmentShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
	QShortcut* showMinimizedShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);

	connect(widgetsHider, &WidgetsHider::widgetsShowed, this, &MessageMediaViewer::updateMediaNavigationButtons);

	connect(nextAttachmentShortcut, &QShortcut::activated, this, &MessageMediaViewer::nextAttachmentButton_clicked);
	connect(previousAttachmentShortcut, &QShortcut::activated, this, &MessageMediaViewer::previousAttachmentButton_clicked);
	connect(showMinimizedShortcut, &QShortcut::activated, this, [this]() {
		showMinimized();
		emit escaped();
		});

	connect(_nextAttachment, &NavigationButton::clicked, this, &MessageMediaViewer::nextAttachmentButton_clicked);
	connect(_previousAttachment, &NavigationButton::clicked, this, &MessageMediaViewer::previousAttachmentButton_clicked);
}

void MessageMediaViewer::updateMediaNavigationButtons() {
	if (!_currentMessage)
		return;

	if (nextMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage)) == -1
		&& _currentMessage->attachmentAt(_currentMessageAttachmentIndex + 1) == nullptr
	) // Впереди нет сообщений с медиа и в текущем сообщении не существует больше медиа
		_nextAttachment->hide();

	if (previousMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage)) == -1
		&& _currentMessage->attachmentAt(_currentMessageAttachmentIndex - 1) == nullptr
	) // Позади нет сообщений с медиа и в текущем сообщении не существует больше медиа
		_previousAttachment->hide();
}

void MessageMediaViewer::updateMessageTextView() {
	if (!_currentMessage)
		return;

	if (!_currentMessage->hasText()) {
		if (!_messageTextView->isHidden())
			_messageTextView->setVisible(false);
		return;
	}

	int yCoordinate;

	if (_messageTextView->isHidden())
		_messageTextView->setVisible(true);

	QSize mediaSize = _mediaPlayer->occupiedMediaSpace();
	QPoint mediaPosition = _mediaPlayer->mediaPosition();

	const int freeBottomSpace = std::max(0, height() - mediaPosition.y() - mediaSize.height());

	_messageTextView->setText(_currentMessage->messageText());

	if (freeBottomSpace >= _messageTextView->height() * 1.5) {
		yCoordinate = height() - (freeBottomSpace / (static_cast<double>(freeBottomSpace) / static_cast<double>(_messageTextView->height()))
			* (static_cast<double>(freeBottomSpace) / static_cast<double>(_messageTextView->height() * 1.5))) - messageTextViewBottomIndent;
		_messageTextView->move((width() - _messageTextView->width()) / 2, yCoordinate);
	} 
	else if ((freeBottomSpace - _messageTextView->height()) >= messageTextViewBottomIndent) {
		yCoordinate = height() - freeBottomSpace - messageTextViewBottomIndent;
		_messageTextView->move((width() - _messageTextView->width()) / 2, yCoordinate);
	}
	else {
		yCoordinate = height() - _messageTextView->height() - messageTextViewBottomIndent;
		if (_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentType().contains("video")) {
			const int videoControlsHeight = _mediaPlayer->getVideoControlsHeight();
			yCoordinate = height() - _messageTextView->height() - messageTextViewBottomIndent - videoControlsHeight;
		}
		_messageTextView->move((width() - _messageTextView->width()) / 2, yCoordinate);
	}
}

void MessageMediaViewer::openMessageAttachment(MessageWidget* messageWidget, int triggeredAttachmentIndex) {
	_currentMessage = messageWidget;
	_currentMessageAttachmentIndex = triggeredAttachmentIndex;
	 
	_mediaPlayer->setVisible(true);
	_mediaPlayer->setSource(QUrl::fromLocalFile(messageWidget->attachmentAt(triggeredAttachmentIndex)->attachmentPath()));

	showNormal();
	_mediaPlayer->showNormal();

	_mediaPlayer->showFullScreen();
	showFullScreen();

	updateMediaNavigationButtons();
	updateMessageTextView();
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

				if (_previousAttachment->isHidden())
					_previousAttachment->show();

				break;
			}
		}
	}

	updateMediaNavigationButtons();
	updateMessageTextView();
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

				if (_nextAttachment->isHidden())
					_nextAttachment->show();

				break;
			}
		}
	}

	updateMediaNavigationButtons();
	updateMessageTextView();
}

void MessageMediaViewer::resizeEvent(QResizeEvent* event) {
	_previousAttachment->move(_nextAttachment->width(), height() / 2);
	_nextAttachment->move(width() - (_previousAttachment->width() * 2), height() / 2);

	updateMessageTextView();
}

void MessageMediaViewer::closeEvent(QCloseEvent* event) {
	emit escaped();
	QWidget::closeEvent(event);
}
