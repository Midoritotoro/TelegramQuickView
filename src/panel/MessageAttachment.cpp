#include "MessageAttachment.h"
#include "MessageWidget.h"

#include <QFontMetrics>
#include <QImage>
#include <QMimeDatabase>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QUrl>
#include <QPixmapCache>


#include "../core/StyleCore.h"

MessageAttachment::MessageAttachment(
	MessageWidget* parentMessage, 
	QString attachmentPath
)
	: ClickableLabel()
	, _attachmentPath(attachmentPath)
	, _attachmentType(detectMediaType(attachmentPath))
	, _parentMessage(parentMessage)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	setCursor(Qt::PointingHandCursor);
}

void MessageAttachment::paintEvent(QPaintEvent* event) {
	const auto _preview = preparePreview();
	if (_preview.isNull())
		return;

	QPainter painter(this);

	switch (_parentMessage->messsageMediaDisplayMode()) {
		case MessageWidget::MessageMediaDisplayMode::Stack:
			painter.drawPixmap(0, 0, _preview);
			break;

		case MessageWidget::MessageMediaDisplayMode::PreviewWithCount:
			if (_parentMessage->attachmentsLength() > 1) {
				painter.drawPixmap(0, 0, _preview);
				painter.setOpacity(0.4);
				painter.fillRect(rect(), Qt::black);

				paintAttachmentCount(painter);
			}
			else {
				painter.drawPixmap(0, 0, _preview);
			}
			break;
	}
}

void MessageAttachment::resizeEvent(QResizeEvent* event) {
	updateSize();
}

void MessageAttachment::paintAttachmentCount(QPainter& painter) {
	const auto attachmentsCountText = "+ " + QString::number(_parentMessage->attachmentsLength() - 1);
	const auto attachmentsCountTextSize = style::TextSize(attachmentsCountText, font());

	QRect attachmentsCountTextRect(QPoint(), attachmentsCountTextSize);
	attachmentsCountTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font());

	painter.setOpacity(1.0);
	painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, attachmentsCountText);
}

QPixmap MessageAttachment::preparePreview() {
	auto preview = QPixmap();
	const auto key = _attachmentPath;

	if (QPixmapCache::find(key, &preview)) 
		return preview;
	else {
		auto image = style::Prepare(QImage(_attachmentPath), size());

		image = std::move(image).scaled(
			image.width() * style::DevicePixelRatio(),
			image.height() * style::DevicePixelRatio(),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);

		image = style::Opaque(std::move(image));

		preview = QPixmap::fromImage(std::move(image), Qt::ColorOnly);
		preview.setDevicePixelRatio(style::DevicePixelRatio());

		if (QPixmapCache::cacheLimit() > 0)
			QPixmapCache::insert(key, preview);
	}

	return preview;
}

void MessageAttachment::updateSize() {
	switch (_attachmentType) {
		case AttachmentType::Photo:
			setFixedSize(
				style::getMinimumSizeWithAspectRatio(
					QPixmap(_attachmentPath).size(),
					style::maximumMessageWidth)
			);
			break;

		case AttachmentType::Video:
			setFixedSize(
				style::getMinimumSizeWithAspectRatio(
					QSize(100, 100),
					style::maximumMessageWidth)
			);
			break;
	}
}

inline void MessageAttachment::setParentMessage(MessageWidget* parentMessage) {
	_parentMessage = parentMessage; 
}

QString MessageAttachment::attachmentPath() const noexcept { 
	return _attachmentPath; 
}

MessageAttachment::AttachmentType MessageAttachment::attachmentType() const noexcept {
	return _attachmentType;
}

MessageWidget* MessageAttachment::parentMessage() const noexcept {
	return _parentMessage; 
}

MessageAttachment::AttachmentType MessageAttachment::detectMediaType(const QString& filePath) noexcept {
	const auto type = QMimeDatabase().mimeTypeForFile(filePath).name();

	return type.contains("audio") ? AttachmentType::Audio
		: type.contains("video") ? AttachmentType::Video
		: type.contains("image") ? AttachmentType::Photo
		: AttachmentType::Unknown;
}