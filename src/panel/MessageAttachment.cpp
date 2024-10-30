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


namespace {
	
	QSize textSize(const QString& text, const QFontMetrics& metrics) {
		return metrics.size(0, text);
	}

	QSize textSize(const QString& text, const QFont& font) {
		return text.isEmpty() ? QSize{} : textSize(text, QFontMetrics(font));
	}
}


MessageAttachment::MessageAttachment(
	QString attachmentPath,
	int attachmentWidth,
	MessageWidget* parentMessage
)
	: ClickableLabel(nullptr)
	, _attachmentPath(attachmentPath)
	, _attachmentType(detectMediaType(attachmentPath))
	, _parentMessage(parentMessage)
	, _attachmentWidth(attachmentWidth)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);
	setCursor(Qt::PointingHandCursor);
}

QSize MessageAttachment::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
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
				painter.setOpacity(0.4);
				painter.drawPixmap(0, 0, _preview);

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
	QFont font("Arial", 16);

	const auto attachmentsCountText = "+ " + QString::number(_parentMessage->attachmentsLength() - 1);
	const auto attachmentsCountTextSize = textSize(attachmentsCountText, font);

	QRect attachmentsCountTextRect(QPoint(), attachmentsCountTextSize);
	attachmentsCountTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font);

	painter.setOpacity(1.0);
	painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, attachmentsCountText);
}

QPixmap MessageAttachment::preparePreview() {
	QPixmap preview;

	QString key = _attachmentPath;
	if (QPixmapCache::find(key, &preview)) {
		return preview;
	}
	else {
		auto image = style::prepare(QImage(_attachmentPath), size());
		preview = QPixmap::fromImage(std::move(image), Qt::ColorOnly);
		preview.setDevicePixelRatio(style::devicePixelRatio());

		if (QPixmapCache::cacheLimit() > 0) {
			QPixmapCache::insert(key, preview);
		}
	}
	return preview;
	
}

void MessageAttachment::updateSize() {
	QSize size;

	switch (_attachmentType) {
		case AttachmentType::Photo:
			size = getMinimumSizeWithAspectRatio(QPixmap(_attachmentPath).size(), _attachmentWidth);
			break;

		case AttachmentType::Video:
			size = getMinimumSizeWithAspectRatio(QSize(100, 100), _attachmentWidth);
			break;
	}

	setFixedSize(size);
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