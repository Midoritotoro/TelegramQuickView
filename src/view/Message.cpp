#include "Message.h"
#include "MessageAttachment.h"

#include "../ui/widgets/FlatLabel.h"
#include "../ui/BasicClickHandlers.h"

#include <QLabel>


Message::Message(
	QWidget* parent
) :
	QWidget(parent)
	, _mediaDisplayMode(MediaDisplayMode::Stack)
{
	setContentsMargins(0, 0, 0, 0);
	setMouseTracking(true);
}

QString Message::text() const noexcept {
	return _textLabel != nullptr
		? _textLabel->text().toString()
		: "";
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
		for (auto index = 0; index < attachments.length(); ++index) {
			const auto display = index == 0;
			auto messageAttachment = new MessageAttachment(this, attachments[index], display);

			_attachments.append(messageAttachment);

			if (display && text.isEmpty() == false) {
				auto _style = *style::defaultFlatLabelStyle;
				_style.maximumWidth = messageAttachment->width();

				const auto st = new style::FlatLabel(_style);
				_textLabel->setStyle(st);
			}
		}

		break;

	case MediaDisplayMode::Stack:
		for (auto index = 0; index < attachments.length(); ++index) {
			auto messageAttachment = new MessageAttachment(this, attachments[index]);

			_attachments.append(messageAttachment);

			if (text.isEmpty() == false) {
				auto _style = *style::defaultFlatLabelStyle;
				_style.maximumWidth = messageAttachment->width();

				const auto st = new style::FlatLabel(_style);
				_textLabel->setStyle(st);
			}
		}

		break;

	case MediaDisplayMode::Album:
		break;
	}

	updateGeometry();
}

const MessageAttachmentsList& Message::attachments() const noexcept {
	return _attachments;
}

QSize Message::sizeHint() const {
	const auto textHeight = _textLabel != nullptr
		? _textLabel->fullHeight()
		: 0;

	const auto maxAttachmentsWidth = maximumAttachmentsWidth();

	const auto hint = QSize(
		maxAttachmentsWidth
			? maxAttachmentsWidth
				: _textLabel != nullptr
				? _textLabel->style()->maximumWidth
			: style::minimumMessageWidth,
		attachmentsHeight() +
		textHeight);

	qDebug() << "this->size(): " << size();
	qDebug() << "hint: " << hint;
	qDebug() << "textHeight: " << textHeight;

	return hint;
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

void Message::resizeEvent(QResizeEvent* event) {
	updateGeometry();
}

void Message::updateGeometry() {
	switch (_mediaDisplayMode) {
		case MediaDisplayMode::Stack:
			for (auto index = 0; index < _attachments.size(); ++index)
				_attachments[index]->move(index == 0
					? QPoint(0, 0)
					: _attachments[index - 1]->rect().bottomLeft());
			break;

		case MediaDisplayMode::PreviewWithCount:
			if (_attachments.isEmpty() == false)
				_attachments[0]->move(0, 0);
			break;

		case MediaDisplayMode::Album:
			break;
	}

	if (_textLabel != nullptr)
		_attachments.isEmpty()
			? _textLabel->move(0, 0)
			: _textLabel->move(_attachments[_attachments.length() - 1]->rect().bottomLeft());
}

int Message::attachmentsHeight() const noexcept {
	auto height = 0;

	if (_attachments.isEmpty())
		return height;

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
	auto width = 0;

	if (_attachments.isEmpty())
		return width;

	switch (_mediaDisplayMode) {
		case MediaDisplayMode::Stack:
			for (auto index = 0; index < _attachments.size(); ++index)
				if (const auto maximumWidth = _attachments[index]->sizeHint().width(); maximumWidth > width)
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