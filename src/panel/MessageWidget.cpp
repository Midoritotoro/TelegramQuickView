#include "MessageWidget.h"
#include "MessageAttachment.h"

#include "FlatLabel.h"

#include <QElapsedTimer>


MessageWidget::MessageWidget(
	QWidget* parent
):
	QWidget(parent)
	, _mediaDisplayMode(MessageMediaDisplayMode::Stack)
{
	_messageLayout = new QGridLayout(this);
	_textLabel = new FlatLabel(this);

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

	_textLabel->setText(text);
	_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_messageLayout->addWidget(_textLabel, _messageLayout->rowCount(), 0, 1, 1, Qt::AlignBottom);
	if (_messageLayout->rowCount() > 1) // У сообщения есть вложение
		_textLabel->setStyleSheet("QLabel{\n"
			"color: white;\n"
			"border-bottom-left-radius: 10px;\n"
			"border-bottom-right-radius: 10px;\n"
		"}");
	else
		_textLabel->setStyleSheet("QLabel{\n"
			"color: white;\n"
			"border-radius: 10px;\n"
		"}");
}

void MessageWidget::addMessageAttachments(const QStringList& attachmentsPaths) {
	if (attachmentsPaths.length() == 0)
		return;

	switch (_mediaDisplayMode) {

	case MessageMediaDisplayMode::PreviewWithCount:
		foreach(const auto& path, attachmentsPaths) {
			auto messageAttachment = new MessageAttachment(this, path);

			if (_messageLayout->rowCount() <= 1)
				_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);

			_attachments.append(messageAttachment);
		}

		break;

	case MessageMediaDisplayMode::Stack:
		foreach(const auto& path, attachmentsPaths) {
			auto messageAttachment = new MessageAttachment(this, path);

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
	return _textLabel->text(); 
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
	return !_textLabel->text().isEmpty();
}