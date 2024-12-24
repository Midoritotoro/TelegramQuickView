#include "Message.h"
#include "MessageAttachment.h"

#include "../ui/widgets/FlatLabel.h"
#include "../ui/BasicClickHandlers.h"


Message::Message(
	QWidget* parent
):
	QWidget(parent)
	, _mediaDisplayMode(MediaDisplayMode::Stack)
{
	_messageLayout = new QVBoxLayout(this);
	_textLabel = new FlatLabel(this);

	_messageLayout->setSpacing(0);

	_messageLayout->setContentsMargins(0, 0, 0, 0);
	setContentsMargins(0, 0, 0, 0);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMouseTracking(true);
}

void Message::setText(const QString& text) {
	if (text.length() == 0)
		return;

	_textLabel->setText(text);

	//if (_messageLayout->count() > 0) // У сообщения есть вложение
	//	_textLabel->setCornerRoundMode(style::CornersRoundMode::Bottom);

	_messageLayout->addWidget(_textLabel, Qt::AlignLeft | Qt::AlignTop);
}

QString Message::text() const noexcept {
	return _textLabel->text().toString();
}

void Message::setAttachments(const QStringList& attachmentsPaths) {
	if (attachmentsPaths.length() == 0)
		return;

	switch (_mediaDisplayMode) {

	case MediaDisplayMode::PreviewWithCount:
		foreach(const auto& path, attachmentsPaths) {
			auto messageAttachment = new MessageAttachment(this, path);

			auto style = *style::defaultFlatLabelStyle;
			style.maximumWidth = messageAttachment->width();

			const auto st = new style::FlatLabel(style);
			_textLabel->setStyle(st);
			
			if (_messageLayout->count() < 1)
				_messageLayout->addWidget(messageAttachment, Qt::AlignLeft | Qt::AlignTop);

			_attachments.append(messageAttachment);
		}

		break;

	case MediaDisplayMode::Stack:
		foreach(const auto& path, attachmentsPaths) {
			auto messageAttachment = new MessageAttachment(this, path);

			auto style = *style::defaultFlatLabelStyle;
			style.maximumWidth = messageAttachment->width();

			const auto st = new style::FlatLabel(style);
			_textLabel->setStyle(st);

			_messageLayout->addWidget(messageAttachment, Qt::AlignLeft | Qt::AlignTop);
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

QSize Message::sizeHint() const {
	return QSize(
		width(),
		attachmentsHeight() + _textLabel->fullHeight());
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

int Message::attachmentsHeight() const noexcept {
	auto height = 0;

	for (auto index = 0; index < _attachments.size(); ++index)
		height += _attachments[index]->height();

	return height;
}
