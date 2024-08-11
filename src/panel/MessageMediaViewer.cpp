#include "MessageMediaViewer.h"

#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QStyle>

#include "History.h"
#include "NavigationButton.h"


MessageMediaViewer::MessageMediaViewer(History* messagesHistory, QWidget* parent)
: QWidget(parent)
, _messagesHistory(messagesHistory)
{
	QToolButton* minimizeWindowButton = new QToolButton();
	QToolButton* maximizeWindowButton = new QToolButton();
	QToolButton* closeWindowButton = new QToolButton();

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget{\n "
		"background-color: rgba(35, 36, 37, 60)\n"
		"}");

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	QPixmap maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);

	minimizeWindowButton->setIcon(minPix);
	maximizeWindowButton->setIcon(maxPix);
	closeWindowButton->setIcon(closePix);

	minimizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	closeWindowButton->setAttribute(Qt::WA_NoSystemBackground);
	maximizeWindowButton->setAttribute(Qt::WA_NoSystemBackground);

	minimizeWindowButton->setStyleSheet("background-color: transparent;");
	closeWindowButton->setStyleSheet("background-color: transparent;");
	maximizeWindowButton->setStyleSheet("background-color: transparent;");

	_grid = new QGridLayout(this);
	_mediaPlayer = new MediaPlayer();
	_mediaPlayer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QWidget* toolWidget = new QWidget();
	QGridLayout* toolLayout = new QGridLayout(toolWidget);
	toolWidget->setObjectName("toolWidget");
	toolWidget->setStyleSheet("#toolWidget{\n"
		"background: transparent;\n"
		"}");
	toolWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	toolWidget->setContentsMargins(0, 0, 0, 0);
	toolLayout->setContentsMargins(0, 0, 0, 0);
	toolLayout->setSpacing(0);

	toolLayout->addWidget(minimizeWindowButton, 0, toolLayout->columnCount(), 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(maximizeWindowButton, 0, toolLayout->columnCount(), 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addWidget(closeWindowButton, 0, toolLayout->columnCount(), 1, 1, Qt::AlignRight | Qt::AlignTop);

	toolLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
	_grid->setSpacing(0);
	_grid->setContentsMargins(0, 0, 0, 0);
	setContentsMargins(0, 0, 0, 0);

	_grid->addWidget(_mediaPlayer, _grid->rowCount(), 0, 1, 1);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir cssDir(currentPath + "/../../assets/images");

	QString arrowPath = cssDir.absolutePath() + "/arrow_right.png";

	_nextAttachment = new NavigationButton(this);
	_previousAttachment = new NavigationButton(this);

	_nextAttachment->setFixedSize(50, 50);
	_previousAttachment->setFixedSize(50, 50);
	
	_nextAttachment->setIcon(QIcon(arrowPath));
	_nextAttachment->setCursor(Qt::PointingHandCursor);

	QTransform transform;
	transform.rotate(180);
	
	_previousAttachment->setIcon(QIcon(QPixmap(arrowPath).transformed(transform)));
	_previousAttachment->setCursor(Qt::PointingHandCursor);

	_previousAttachment->move(_nextAttachment->width(), height() / 2);
	_nextAttachment->move(width() - (_previousAttachment->width() * 2), height() / 2);

	_nextAttachment->show();
	_previousAttachment->show();
	setContentsMargins(0, 0, 0, 0);

	_mediaPlayer->setVisible(false);

	connect(minimizeWindowButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeWindowButton, &QToolButton::clicked, this, &QWidget::close);
	connect(maximizeWindowButton, &QToolButton::clicked, this, [this]() {
		isFullScreen() ? showNormal() : showFullScreen();
		});

	connect(_nextAttachment, &NavigationButton::clicked, this, &MessageMediaViewer::toNext);
	connect(_previousAttachment, &NavigationButton::clicked, this, &MessageMediaViewer::toPrevious);
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

bool MessageMediaViewer::isNextMediaAvailable() {
	return true;
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
	updateMediaNavigationButtons();
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
	updateMediaNavigationButtons();
}

void MessageMediaViewer::toNext() {
	int messageAttachmentsCount = _currentMessage->attachmentsLength();

	for (int index = 0; index < messageAttachmentsCount; ++index) {
		if ((messageAttachmentsCount - _currentMessageAttachmentIndex) <= 0) {
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

	if ((messageAttachmentsCount - _currentMessageAttachmentIndex) <= 1) 
		// Медиа в текущем сообщении нет, идём к следующему
		goToNextMessage();
}

void MessageMediaViewer::toPrevious() {
	int messageAttachmentsCount = _currentMessage->attachmentsLength();

	for (int index = messageAttachmentsCount; index >= 0; --index) {
		if (_currentMessageAttachmentIndex <= 0)
			break;

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

	if (_currentMessageAttachmentIndex <= 0)
		// Медиа в текущем сообщении нет, идём к предыдущему
		goToPreviousMessage();
}

void MessageMediaViewer::resizeEvent(QResizeEvent* event) {
	_previousAttachment->move(_nextAttachment->width(), height() / 2);
	_nextAttachment->move(width() - (_previousAttachment->width() * 2), height() / 2);
}
