#include "MessageAttachment.h"
#include "Message.h"

#include <QMimeDatabase>
#include <QPainter>

#include "../ui/style/StyleCore.h"
#include "../core/Time.h"

#include <QPaintEvent>


MessageAttachment::MessageAttachment(
	not_null<Message*> parentMessage,
	const QString& attachmentPath
)
	: QAbstractButton()
	, _attachmentPath(attachmentPath)
	, _attachmentType(detectMediaType(attachmentPath))
	, _parentMessage(parentMessage)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	setCursor(Qt::PointingHandCursor);
}

void MessageAttachment::paintEvent(QPaintEvent* event) {
	const auto ms = Time::now();
	const auto timer = gsl::finally([=] { qDebug() << "MessageAttachment::paintEvent: " << Time::now() - ms << " ms"; });

	const auto _preview = style::GenerateThumbnail(_attachmentPath, size());
	if (_preview.isNull())
		return;

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

	switch (_parentMessage->mediaDisplayMode()) {
		case Message::MediaDisplayMode::Stack:
			if (_parentMessage->attachmentsLength() == 1 && _parentMessage->hasText() == false)
				style::RoundCorners(painter, size(), 15);
			else if (_parentMessage->attachmentsLength() > 1 && _parentMessage->indexOfAttachment(this) == 0)
				style::RoundTopCorners(painter, size(), 15);

			painter.drawPixmap(0, 0, _preview);
			break;

		case Message::MediaDisplayMode::PreviewWithCount:
			_parentMessage->hasText()
				? style::RoundTopCorners(painter, size(), 15)
				: style::RoundCorners(painter, size(), 15);

			if (_parentMessage->attachmentsLength() > 1) {
				painter.drawPixmap(0, 0, _preview);

				painter.setOpacity(0.4);
				painter.fillRect(rect(), Qt::black);

				paintAttachmentCount(painter);
			}
			else
				painter.drawPixmap(0, 0, _preview);

			break;
	}
}

void MessageAttachment::resizeEvent(QResizeEvent* event) {
	updateSize();
}

void MessageAttachment::paintAttachmentCount(QPainter& painter) {
	const auto attachmentsCountText = "+" + QString::number(_parentMessage->attachmentsLength() - 1);
	const auto attachmentsCountTextSize = style::TextSize(attachmentsCountText, font());

	QRect attachmentsCountTextRect(QPoint(), attachmentsCountTextSize);
	attachmentsCountTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font());

	painter.setOpacity(1.0);
	painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, attachmentsCountText);
}

void MessageAttachment::updateSize() {
	setFixedSize(
		style::getMinimumSizeWithAspectRatio(
			style::GenerateThumbnail(_attachmentPath).size(),
			style::maximumMessageWidth)
	);
}

void MessageAttachment::setParentMessage(not_null<Message*> parentMessage) {
	_parentMessage = parentMessage; 
}

QString MessageAttachment::attachmentPath() const noexcept { 
	return _attachmentPath; 
}

MessageAttachment::AttachmentType MessageAttachment::attachmentType() const noexcept {
	return _attachmentType;
}

Message* MessageAttachment::parentMessage() const noexcept {
	return _parentMessage; 
}

MessageAttachment::AttachmentType MessageAttachment::detectMediaType(const QString& filePath) noexcept {
	const auto type = QMimeDatabase().mimeTypeForFile(filePath).name();

	return type.contains("audio") ? AttachmentType::Audio
		: type.contains("video") ? AttachmentType::Video
		: type.contains("image") ? AttachmentType::Photo
		: AttachmentType::Unknown;
}