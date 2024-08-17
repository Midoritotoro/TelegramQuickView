#include "MessageAttachment.h"
#include "MessageWidget.h"

#include <QMimeDatabase>
#include <QUrl>
#include <QPainter>
#include <QFontMetrics>


namespace {
	QSize textSize(const QString& _text, const QFontMetrics& _metrcis) {
		return _metrcis.size(0, _text);
	}

	QSize textSize(const QString& _text, const QFont& _font) {
		return _text.isEmpty() ? QSize{} : textSize(_text, QFontMetrics(_font));
	}
}


MessageAttachment::MessageAttachment(
	QString attachmentPath,
	int attachmentWidth,
	MessageWidget* parentMessage,
	QWidget* parent
)
: ClickableLabel(parent)
, _attachmentPath(attachmentPath)
, _attachmentType(detectMediaType(attachmentPath))
, _parentMessage(parentMessage)
, _attachmentWidth(attachmentWidth)
{
	setMouseTracking(true);
	setScaledContents(true);
	setBackgroundRole(QPalette::Dark);
	setCursor(Qt::PointingHandCursor);

	if (_attachmentType.contains("image"))
		_attachmentPreview.load(attachmentPath);

	updatePreviewSize();
}

QSize MessageAttachment::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageAttachment::setParentMessage(MessageWidget* parentMessage) {
	_parentMessage = parentMessage;
}

QString MessageAttachment::attachmentPath() const noexcept {
	return _attachmentPath;
}

QString MessageAttachment::attachmentType() const noexcept {
	return _attachmentType;
}

MessageWidget* MessageAttachment::parentMessage() const noexcept {
	return _parentMessage;
}

QString MessageAttachment::detectMediaType(const QString& filePath) {
	return QMimeDatabase().mimeTypeForFile(filePath).name();
}

void MessageAttachment::paintEvent(QPaintEvent* event) {
	QLabel::paintEvent(event);

	if (_attachmentPreview.isNull())
		return;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	
	switch (_parentMessage->messsageMediaDisplayMode()) {
		case MessageWidget::MessageMediaDisplayMode::Stack:
			painter.drawImage(0, 0, _attachmentPreview);
			setFixedSize(_attachmentPreview.size());
			break;
		case MessageWidget::MessageMediaDisplayMode::PreviewWithCount:
			if ((_parentMessage->attachmentsLength() - 1) > 0) {
				painter.setOpacity(0.4);
				painter.drawImage(0, 0, _attachmentPreview);
				setFixedSize(_attachmentPreview.size());

				painter.setPen(Qt::lightGray);
				QFont font("Arial", 18);

				const QString attachmentsCountText = "+ " + QString::number(_parentMessage->attachmentsLength() - 1);
				const QSize attachmentsCountTextSize = textSize(attachmentsCountText, font);

				QRect attachmentsCountTextRect(QPoint(), attachmentsCountTextSize);
				attachmentsCountTextRect.moveCenter(rect().center());

				painter.setOpacity(1.0);
				painter.setFont(font);
				painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, attachmentsCountText);
			}
			else {
				painter.drawImage(0, 0, _attachmentPreview);
				setFixedSize(_attachmentPreview.size());
			}
			break;
	}
}

void MessageAttachment::resizeEvent(QResizeEvent* event) {
	ClickableLabel::resizeEvent(event);
	updatePreviewSize();
}

void MessageAttachment::updatePreviewSize() {
	if (_attachmentType.contains("image")) {
		if (_attachmentPreview.format() != QImage::Format_ARGB32_Premultiplied)
			_attachmentPreview = _attachmentPreview.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		QSize size = getMinimumSizeWithAspectRatio(_attachmentPreview.size(), _attachmentWidth);
		_attachmentPreview = _attachmentPreview.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	else if (_attachmentType.contains("video")) {
		_attachmentPreview = QImage(_attachmentWidth / 2, _attachmentWidth / 2, QImage::Format_ARGB32_Premultiplied);
		_attachmentPreview = _attachmentPreview.scaled(Qt::IgnoreAspectRatio, Qt::FastTransformation);
		_attachmentPreview.fill(Qt::white);
	}
}