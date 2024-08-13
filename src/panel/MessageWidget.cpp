#include "MessageWidget.h"
#include "MessageAttachment.h"

#include <QGridLayout>


MessageWidget::MessageWidget(const QString& author, QWidget* parent) :
	QWidget(parent)
{
	_telegramMessage = new TelegramMessage();
	_telegramMessage->author = author;

	_messageLayout = new QGridLayout(this);
	setContentsMargins(0, 0, 0, 0);
	_messageLayout->setVerticalSpacing(0);
	_messageLayout->setContentsMargins(0, 0, 0, 0);

	setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
	"}");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMouseTracking(true);
}

void MessageWidget::addMessageText(const QString& text) {
	if (text.length() == 0)
		return;

	QLabel* textLabel = new QLabel();

	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignLeft);
	textLabel->setContentsMargins(8, 5, 20, 8);

	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
	textLabel->setText(text);
	_telegramMessage->text = text;

	if (_messageLayout->rowCount() > 1) // У сообщения есть вложение
		textLabel->setStyleSheet("QLabel{\n"
			"color: white;\n"
			"border-bottom-left-radius: 10px;\n"
			"border-bottom-right-radius: 10px;\n"
		"}");
	else
		textLabel->setStyleSheet("QLabel{\n"
			"color: white;\n"
			"border-radius: 10px;\n"
		"}");


	_messageLayout->addWidget(textLabel, _messageLayout->rowCount(), 0, 1, 1);
}

void MessageWidget::addMessageAttachments(const QUrlList& attachmentsPaths, int maximumMessageWidth) {

	foreach(const QUrl& url, attachmentsPaths) {
		QString sourcePath;

		if (url.path().at(0) == "/"[0])
			sourcePath = url.path().remove(0, 1);
		else
			sourcePath = url.path();

		MessageAttachment* messageAttachment = new MessageAttachment(sourcePath, maximumMessageWidth);
		_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);
		messageAttachment->setParentMessage(this);
		
		_telegramMessage->attachments.append(messageAttachment);
	}
}

QString MessageWidget::messageText() const noexcept {
	return _telegramMessage->text;	
}

const MessageAttachmentsList& MessageWidget::messageAttachments() const noexcept {
	return _telegramMessage->attachments;
}

int MessageWidget::indexOfAttachment(MessageAttachment* messageAttachment) const noexcept {
	return _telegramMessage->attachments.indexOf(messageAttachment);
}

MessageAttachment* MessageWidget::attachmentAt(int index) const noexcept {
	if (index >= 0 && attachmentsLength() > index)
		return _telegramMessage->attachments.at(index);
	return nullptr;
}

int MessageWidget::attachmentsLength() const noexcept {
	return _telegramMessage->attachments.length();
}

bool MessageWidget::hasAttachments() const noexcept {
	return !_telegramMessage->attachments.isEmpty();
}

bool MessageWidget::hasText() const noexcept {
	return !_telegramMessage->text.isEmpty();
}