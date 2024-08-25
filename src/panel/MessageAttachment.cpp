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

QString MessageAttachment::detectMediaType(const QString& filePath) noexcept {
	return QMimeDatabase().mimeTypeForFile(filePath).name();
}

void MessageAttachment::paintEvent(QPaintEvent* event) {
	QLabel::paintEvent(event);

	QPixmap preview = QPixmap(_attachmentPath);

	if (preview.isNull())
		return;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	switch (_parentMessage->messsageMediaDisplayMode()) {

	case MessageWidget::MessageMediaDisplayMode::Stack:
		painter.drawPixmap(0, 0, preview.size() != _attachmentPreviewSize ? preview.scaled(_attachmentPreviewSize, Qt::KeepAspectRatio, Qt::SmoothTransformation) : preview);
		break;

	case MessageWidget::MessageMediaDisplayMode::PreviewWithCount:
		if ((_parentMessage->attachmentsLength() - 1) > 0) {
			painter.setOpacity(0.4);
			painter.drawPixmap(0, 0, preview.size() != _attachmentPreviewSize ? preview.scaled(_attachmentPreviewSize, Qt::KeepAspectRatio, Qt::SmoothTransformation) : preview);

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
			painter.drawPixmap(0, 0, preview.size() != _attachmentPreviewSize ? preview.scaled(_attachmentPreviewSize, Qt::KeepAspectRatio, Qt::SmoothTransformation) : preview);
		}
		break;
	}
}

void MessageAttachment::resizeEvent(QResizeEvent* event) {
	ClickableLabel::resizeEvent(event);

	updatePreviewSize();
	setFixedSize(_attachmentPreviewSize);
}

void MessageAttachment::updatePreviewSize() {
	if (_attachmentType.contains("image")) {
		QSize size = getMinimumSizeWithAspectRatio(QPixmap(_attachmentPath).size(), _attachmentWidth);
		_attachmentPreviewSize = size;
	}
	else if (_attachmentType.contains("video")) {
		QSize size = getMinimumSizeWithAspectRatio(QSize(100, 100), _attachmentWidth);
		_attachmentPreviewSize = size;
	}
}