#include "MediaViewer.h"

#include "../ui/widgets/FlatButton.h"
#include "../ui/WidgetsHider.h"

#include "Message.h"

#include "TextView.h"
#include "MessageAttachment.h"

#include "Message.h"

#include <QShortcut>
#include <QKeySequence>

#include <QDir>
#include <QApplication>


namespace {
	constexpr int messageTextViewBottomIndent = 5;
}


MediaViewer::MediaViewer(
	not_null<History*> messagesHistory,
	QWidget* parent
): 
	QWidget(parent)
	, _messagesHistory(messagesHistory)
{
	setMouseTracking(true);

	const int screenWidth = QApplication::primaryScreen()->availableGeometry().height();

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("QWidget{\n"
		"background-color: rgba(15, 15, 15, 99)\n"
	"}");


	QString arrowPath = ":/assets/images/arrow_right.png";

	_grid = new QGridLayout(this);
	_mediaPlayer = std::make_unique<MediaPlayer>();
	_mediaPlayer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_grid->setSpacing(0);
	_grid->setContentsMargins(0, 0, 0, 0);
	setContentsMargins(0, 0, 0, 0);

	_grid->addWidget(_mediaPlayer.get(), _grid->rowCount(), 0, 1, 1);

	QTransform transform;
	transform.rotate(180);

	_nextAttachment = new FlatButton(this);
	_previousAttachment = new FlatButton(this);
	_messageTextView = new TextView(this);

	_nextAttachment->setFixedSize(38, 38);
	_previousAttachment->setFixedSize(38, 38);

	_previousAttachment->setButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
	_nextAttachment->setButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);

	_nextAttachment->setIconSize(QSize(38, 38));
	_previousAttachment->setIconSize(QSize(38, 38));

	_nextAttachment->setIcon(QIcon(arrowPath));
	_nextAttachment->setCursor(Qt::PointingHandCursor);

	_previousAttachment->setIcon(QIcon(QPixmap(arrowPath).transformed(transform)));
	_previousAttachment->setCursor(Qt::PointingHandCursor);

	_mediaPlayer->setVisible(false);

	_previousAttachment->setVisible(true);
	_nextAttachment->setVisible(true);
	_messageTextView->setVisible(false);

	_messageTextView->setMaximumWidth(screenWidth * 0.8);

	const auto widgetsList = QWidgetList({ _previousAttachment, _nextAttachment, /* _messageTextViev */ });
	_widgetsHider =  std::make_unique<WidgetsHider>(true, true, widgetsList);

	_widgetsHider->SetInactivityDuration(3000);
	_widgetsHider->SetAnimationDuration(3000);
	
	QShortcut* nextAttachmentShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
	QShortcut* previousAttachmentShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
	QShortcut* showMinimizedShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);

	connect(_widgetsHider.get(), &WidgetsHider::widgetsShowed, this, &MediaViewer::updateMediaNavigationButtons);

	connect(nextAttachmentShortcut, &QShortcut::activated, this, &MediaViewer::nextAttachmentButton_clicked);
	connect(previousAttachmentShortcut, &QShortcut::activated, this, &MediaViewer::previousAttachmentButton_clicked);
	connect(showMinimizedShortcut, &QShortcut::activated, this, [this]() {
		showMinimized();
		emit escaped();
		_mediaPlayer->cleanUp();
		});

	connect(_nextAttachment, &QAbstractButton::clicked, this, &MediaViewer::nextAttachmentButton_clicked);
	connect(_previousAttachment, &QAbstractButton::clicked, this, &MediaViewer::previousAttachmentButton_clicked);

	connect(_mediaPlayer.get(), &MediaPlayer::mediaGeometryChanged, this, &MediaViewer::updateMessageTextView);
	connect(_mediaPlayer.get(), &MediaPlayer::needScrollToMessage, [this]() {
		showMinimized();

		emit escaped();
		emit needScrollToMessage(_currentMessage);
	});
}

void MediaViewer::updateMediaNavigationButtons() {
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

void MediaViewer::updateMessageTextView() {
	if (!_currentMessage)
		return;

	if (!_currentMessage->hasText()) {
		if (!_messageTextView->isHidden()) {
			_messageTextView->hide();
			_widgetsHider->removeWidget(_messageTextView);
		}
		return;
	}

	if (_messageTextView->isHidden())
		_messageTextView->show();

	_widgetsHider->addWidget(_messageTextView);
	_messageTextView->setText(_currentMessage->text());

	auto yCoordinate = 0;
	
	const auto mediaSize = _mediaPlayer->occupiedMediaSpace();
	const auto mediaPosition = _mediaPlayer->mediaPosition();

	const auto videoControlsHeight = _mediaPlayer->getVideoControlsHeight();

	const auto freeBottomSpace = qMax(0, mediaPosition.y() > 0 ?
					height() - mediaPosition.y() - mediaSize.height()
					- videoControlsHeight - messageTextViewBottomIndent
					- _messageTextView->height() : 0);

	const auto bottomFreeSpaceToTextViewHeightRatio = static_cast<double>(freeBottomSpace) / static_cast<double>(_messageTextView->height());

	if (freeBottomSpace > _messageTextView->height()) {
		// Виджет с текстом полностью помещается в свободное простанство по высоте
		yCoordinate = height() - (freeBottomSpace / bottomFreeSpaceToTextViewHeightRatio)
			* bottomFreeSpaceToTextViewHeightRatio - messageTextViewBottomIndent
			- (videoControlsHeight * bottomFreeSpaceToTextViewHeightRatio);
	} 
	else {
		yCoordinate = height() - _messageTextView->height() 
			- messageTextViewBottomIndent * 2. - videoControlsHeight;
	}

	_messageTextView->hide();
	_messageTextView->move((width() - _messageTextView->width()) / 2., yCoordinate);
}

void MediaViewer::openMessageAttachment(
	not_null<Message*> message,
	int triggeredAttachmentIndex)
{
	_currentMessage = message;
	_currentMessageAttachmentIndex = triggeredAttachmentIndex;

	_mediaPlayer->showFullScreen();
	showFullScreen();

	if (_mediaPlayer->isHidden())
		_mediaPlayer->setVisible(true);

	_mediaPlayer->setMedia(message->attachmentAt(triggeredAttachmentIndex)->attachmentPath());

	updateMediaNavigationButtons();
	updateMessageTextView();
}

int MediaViewer::nextMessageWithAttachmentsIndex(int currentIndex) const noexcept {
	for (int i = currentIndex + 1; i < _messagesHistory->count(); ++i)
		if (_messagesHistory->messageAt(i)->hasAttachments())
			return i;
	return -1;
}

int MediaViewer::previousMessageWithAttachmentsIndex(int currentIndex) const noexcept {
	for (int i = currentIndex - 1; i >= 0; --i)
		if (_messagesHistory->messageAt(i)->hasAttachments())
			return i;
	return -1;
}

void MediaViewer::goToPreviousMessage() {
	const auto previousMessageIndex = previousMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage));

	if (previousMessageIndex < 0)
		return;

	if (const auto previousMessage = _messagesHistory->messageAt(previousMessageIndex)) {
		_currentMessage = previousMessage;

		_currentMessageAttachmentIndex = _currentMessage->attachmentsLength() - 1;
		_mediaPlayer->setMedia(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath());

		if (_nextAttachment->isHidden())
			_nextAttachment->show();
	}
}

void MediaViewer::goToNextMessage() {
	const auto nextMessageIndex = nextMessageWithAttachmentsIndex(_messagesHistory->indexOfMessage(_currentMessage));

	if (nextMessageIndex < 0)
		return;

	if (const auto nextMessage = _messagesHistory->messageAt(nextMessageIndex)) {
		_currentMessage = nextMessage;
				
		_currentMessageAttachmentIndex = 0;
		_mediaPlayer->setMedia(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath());

		if (_previousAttachment->isHidden())
			_previousAttachment->show();
	}
}

void MediaViewer::nextAttachmentButton_clicked() {
	const auto attachmentsCount = _currentMessage->attachmentsLength();

	for (int index = 0; index < attachmentsCount; ++index) {
		if ((attachmentsCount - (_currentMessageAttachmentIndex + 1)) <= 0) {
			// Медиа в текущем сообщении нет, идём к следующему
			goToNextMessage();
			break;
		}

		const auto attachment = _currentMessage->attachmentAt(_currentMessageAttachmentIndex);

		if (_currentMessage->indexOfAttachment(attachment) != index || attachmentsCount - (index + 1) <= 0)
			continue;

		_currentMessageAttachmentIndex = ++index;
		_mediaPlayer->setMedia(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath());

		if (_previousAttachment->isHidden())
			_previousAttachment->show();

		break;
	}

	updateMediaNavigationButtons();
	updateMessageTextView();
}

void MediaViewer::previousAttachmentButton_clicked() {
	const auto attachmentsCount = _currentMessage->attachmentsLength();

	for (int index = attachmentsCount; index >= 0; --index) {
		if (_currentMessageAttachmentIndex <= 0) {
			// Медиа в текущем сообщении нет, идём к предыдущему
			goToPreviousMessage();
			break;
		}

		const auto attachment = _currentMessage->attachmentAt(_currentMessageAttachmentIndex);

		if (_currentMessage->indexOfAttachment(attachment) != index || index - 1 < 0)
			continue;
			
		_currentMessageAttachmentIndex = --index;
		_mediaPlayer->setMedia(_currentMessage->attachmentAt(_currentMessageAttachmentIndex)->attachmentPath());

		if (_nextAttachment->isHidden())
			_nextAttachment->show();

		break;
	}

	updateMediaNavigationButtons();
	updateMessageTextView();
}

void MediaViewer::resizeEvent(QResizeEvent* event) {
	_previousAttachment->move(_nextAttachment->width(), height() / 2);
	_nextAttachment->move(width() - (_previousAttachment->width() * 2), height() / 2);

	updateMessageTextView();
}

void MediaViewer::closeEvent(QCloseEvent* event) {
	emit escaped();
	_mediaPlayer->cleanUp();

	QWidget::closeEvent(event);
}
