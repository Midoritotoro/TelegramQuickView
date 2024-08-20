#include "MessageAttachment.h"
#include "MessageWidget.h"

#include <QFontMetrics>
#include <QImage>
#include <QMimeDatabase>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QUrl>

#include <QElapsedTimer>


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
	setScaledContents(true);
	setBackgroundRole(QPalette::Dark);
	setCursor(Qt::PointingHandCursor);
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

QString MessageAttachment::detectMediaType(const QString& filePath) noexcept {
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

	if (_attachmentType.contains("image")
		&& _attachmentPreview.isNull())
		_attachmentPreview = QImage(_attachmentPath).convertToFormat(QImage::Format_ARGB32_Premultiplied);
	else if (_attachmentType.contains("video") 
		&& _attachmentPreview.isNull())
		_attachmentPreview = QImage(_attachmentWidth / 2, _attachmentWidth / 2, QImage::Format_ARGB32_Premultiplied);

	updatePreviewSize();
}

void MessageAttachment::updatePreviewSize() {
	if (_attachmentType.contains("image")) {
		QSize size = getMinimumSizeWithAspectRatio(QImage(_attachmentPath).size(), _attachmentWidth);
		if (_attachmentPreview.size() != size)
			_attachmentPreview = _attachmentPreview.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	else if (_attachmentType.contains("video")) {
		_attachmentPreview.fill(Qt::white);
		QSize size = getMinimumSizeWithAspectRatio(QSize(100, 100), _attachmentWidth);
		_attachmentPreview = _attachmentPreview.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
}