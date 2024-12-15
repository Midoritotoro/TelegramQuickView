#include "Message.h"
#include "MessageAttachment.h"

#include "../ui/widgets/FlatLabel.h"
#include "../ui/ClickHandler.h"


Message::Message(
	QWidget* parent
):
	QWidget(parent)
	, _mediaDisplayMode(MediaDisplayMode::Stack)
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

void Message::setText(const QString& text) {
	if (text.length() == 0)
		return;

	auto handler = std::make_shared<UrlClickHandler>("https://google.com");

	_textLabel->setText(text);
	_textLabel->setLink(1, handler);

	qDebug() << "hasLinks: " << _textLabel->hasLinks();

	_messageLayout->addWidget(_textLabel, _messageLayout->rowCount(), 0, 1, 1, Qt::AlignBottom);
	if (_messageLayout->rowCount() > 1) // У сообщения есть вложение
		_textLabel->setCornerRoundMode(style::CornersRoundMode::Bottom);
}

QString Message::text() const noexcept {
	return _textLabel->text();
}


void Message::setAttachments(const QStringList& attachmentsPaths) {
	if (attachmentsPaths.length() == 0)
		return;

	switch (_mediaDisplayMode) {

	case MediaDisplayMode::PreviewWithCount:
		foreach(const auto& path, attachmentsPaths) {
			auto messageAttachment = new MessageAttachment(this, path);

			if (_messageLayout->rowCount() <= 1)
				_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);

			_attachments.append(messageAttachment);
		}

		break;

	case MediaDisplayMode::Stack:
		foreach(const auto& path, attachmentsPaths) {
			auto messageAttachment = new MessageAttachment(this, path);

			_messageLayout->addWidget(messageAttachment, _messageLayout->rowCount(), 0, 1, 1);
			_attachments.append(messageAttachment);
		}
		break;

	case MediaDisplayMode::Album:
		break;
	}
}

const MessageAttachmentsList& Message::attachments() const noexcept {
	return _attachments;
}


void Message::setMediaDisplayMode(MediaDisplayMode displayMode) {
	_mediaDisplayMode = displayMode;
}

Message::MediaDisplayMode Message::mediaDisplayMode() const noexcept {
	return _mediaDisplayMode; 
}

int Message::indexOfAttachment(not_null<MessageAttachment*> messageAttachment) const noexcept {
	return _attachments.indexOf(messageAttachment); 
}

MessageAttachment* Message::attachmentAt(int index) const noexcept {
	if (index >= 0 && attachmentsLength() > index)
		return _attachments.at(index);
	return nullptr;
}

int Message::attachmentsLength() const noexcept {
	return _attachments.length();
}

bool Message::hasAttachments() const noexcept {
	return !_attachments.isEmpty();
}

bool Message::hasText() const noexcept {
	return !_textLabel->text().isEmpty();
}