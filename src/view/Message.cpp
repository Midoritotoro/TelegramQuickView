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

	_messageLayout->setSpacing(0);

	_messageLayout->setContentsMargins(0, 0, 0, 0);

	setContentsMargins(0, 0, 0, 0);
	setMouseTracking(true);
}

void Message::setAttributes(
	const QString& text,
	const QStringList& attachments)
{
	if (text.isEmpty() == false) {
		_textLabel = new FlatLabel(this);
		_textLabel->setText(text);
	}

	if (attachments.count() > 0)
		_textLabel->setCornerRoundMode(style::CornersRoundMode::Bottom);

	switch (_mediaDisplayMode) {

	case MediaDisplayMode::PreviewWithCount:
		foreach(const auto & path, attachments) {
			const auto display = _messageLayout->count() < 1;
			auto messageAttachment = new MessageAttachment(this, path, display);

			if (display)
				_messageLayout->addWidget(messageAttachment, Qt::AlignLeft | Qt::AlignTop);

			_attachments.append(messageAttachment);

			if (text.isEmpty() || display == false)
				continue;

			auto _style = *style::defaultFlatLabelStyle;
			_style.maximumWidth = messageAttachment->width();

			const auto st = new style::FlatLabel(_style);
			_textLabel->setStyle(st);
		}

		break;

	case MediaDisplayMode::Stack:
		foreach(const auto & path, attachments) {
			auto messageAttachment = new MessageAttachment(this, path);

			_messageLayout->addWidget(messageAttachment, Qt::AlignLeft | Qt::AlignTop);
			_attachments.append(messageAttachment);

			if (text.isEmpty())
				continue;

			auto _style = *style::defaultFlatLabelStyle;
			_style.maximumWidth = messageAttachment->width();

			const auto st = new style::FlatLabel(_style);
			_textLabel->setStyle(st);
		}

		break;

	case MediaDisplayMode::Album:
		break;
	}

	_messageLayout->addWidget(_textLabel, Qt::AlignLeft | Qt::AlignTop);
}


QString Message::text() const noexcept {
	return _textLabel != nullptr
		? _textLabel->text().toString()
		: "";
}

const MessageAttachmentsList& Message::attachments() const noexcept {
	return _attachments;
}

QSize Message::sizeHint() const {
	const auto textHeight = _textLabel != nullptr
		? _textLabel->fullHeight()
		: 0;

	qDebug() << "textHeight: " << textHeight;

	return QSize(
		maximumAttachmentsWidth(),
		attachmentsHeight() + 
		textHeight);
}

QSize Message::minimumSizeHint() const {
	return sizeHint();
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
	return _textLabel != nullptr && !_textLabel->text().isEmpty();
}

int Message::attachmentsHeight() const noexcept {
	if (_attachments.isEmpty())
		return 0;

	auto height = 0;

	switch (_mediaDisplayMode) {
		case MediaDisplayMode::Stack:
			for (auto index = 0; index < _attachments.size(); ++index)
				height += _attachments[index]->sizeHint().height();

			break;

		case MediaDisplayMode::PreviewWithCount:
			height += _attachments[0]->sizeHint().height();
			break;

		case MediaDisplayMode::Album:
			break;
	}
	return height;
}

int Message::maximumAttachmentsWidth() const noexcept {
	if (_attachments.isEmpty())
		return 0;

	auto width = 0;

	switch (_mediaDisplayMode) {
		case MediaDisplayMode::Stack:
			for (auto index = 0; index < _attachments.size(); ++index)
				if (const auto maximumWidth = _attachments[index]->sizeHint().width();
					maximumWidth > width)
					width = maximumWidth;
			break;

		case MediaDisplayMode::PreviewWithCount:
			width = _attachments[0]->sizeHint().width();
			break;

		case MediaDisplayMode::Album:
			break;
	}

	return width;
}