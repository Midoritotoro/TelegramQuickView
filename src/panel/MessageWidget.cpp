#include "MessageWidget.h"
#include "MessageAttachment.h"

#include <QElapsedTimer>


MessageWidget::MessageWidget(
	QWidget* parent
):
QWidget(parent)
, _mediaDisplayMode(MessageMediaDisplayMode::Stack)
{
	_messageLayout = new QGridLayout(this);

	setContentsMargins(0, 0, 0, 0);

	_messageLayout->setSpacing(0);
	_messageLayout->setContentsMargins(0, 0, 0, 0);

	setStyleSheet("QWidget{\n"
		"background: rgb(24, 37, 51);\n"
		"border: 5px;\n"
	"}");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MessageWidget::addMessageText(const QString& text) {
	if (text.length() == 0)
		return;

	QLabel* textLabel = new QLabel();

	textLabel->setWordWrap(true);
	textLabel->setAlignment(Qt::AlignLeft);
	textLabel->setContentsMargins(8, 5, 20, 10);

	textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

	textLabel->setText(text);
	_text = text;

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

	_messageLayout->addWidget(textLabel, _messageLayout->rowCount(), 0, 1, 1, Qt::AlignBottom);
}

void MessageWidget::addMessageAttachments(const QStringList& attachmentsPaths, int maximumMessageWidth) {
	if (attachmentsPaths.length() == 0)
		return;

	switch (_mediaDisplayMode) {

	case MessageMediaDisplayMode::PreviewWithCount:
		foreach(const auto& path, attachmentsPaths) {
			MessageAttachment* messageAttachment = new MessageAttachment(path, maximumMessageWidth, this);
			messageAttachment->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

			if (_messageLayout->rowCount() <= 1)
				_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);

			_attachments.append(messageAttachment);
		}

		break;

	case MessageMediaDisplayMode::Stack:
		foreach(const auto& path, attachmentsPaths) {
			MessageAttachment* messageAttachment = new MessageAttachment(path, maximumMessageWidth, this);
			_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);

			_attachments.append(messageAttachment);
		}
		break;

	case MessageMediaDisplayMode::Album:
		break;
	}
}

void MessageWidget::setMessageMediaDisplayMode(MessageMediaDisplayMode displayMode) {
	_mediaDisplayMode = displayMode;
}

MessageWidget::MessageMediaDisplayMode MessageWidget::messsageMediaDisplayMode() const noexcept {
	return _mediaDisplayMode; 
}

QString MessageWidget::messageText() const noexcept {
	return _text; 
}

const MessageAttachmentsList& MessageWidget::messageAttachments() const noexcept { 
	return _attachments; 
}

int MessageWidget::indexOfAttachment(MessageAttachment* messageAttachment) const noexcept {
	return _attachments.indexOf(messageAttachment); 
}

MessageAttachment* MessageWidget::attachmentAt(int index) const noexcept {
	if (index >= 0 && attachmentsLength() > index)
		return _attachments.at(index);
	return nullptr;
}

int MessageWidget::attachmentsLength() const noexcept {
	return _attachments.length();
}

bool MessageWidget::hasAttachments() const noexcept {
	return !_attachments.isEmpty();
}

bool MessageWidget::hasText() const noexcept {
	return !_text.isEmpty();
}