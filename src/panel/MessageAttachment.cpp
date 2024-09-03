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


//void AlbumThumbnail::paintPlayVideo(QPainter& p, QRect geometry) {
//	const auto innerSize = st::msgFileLayout.thumbSize;
//	const auto inner = QRect(
//		geometry.x() + (geometry.width() - innerSize) / 2,
//		geometry.y() + (geometry.height() - innerSize) / 2,
//		innerSize,
//		innerSize);
//	{
//		PainterHighQualityEnabler hq(p);
//		p.setPen(Qt::NoPen);
//		p.setBrush(st::msgDateImgBg);
//		p.drawEllipse(inner);
//	}
//	st::historyFileThumbPlay.paintInCenter(p, inner);
//}

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

	QPixmap preview;

	if (_attachmentType.contains("image")) {
		if (!QPixmapCache::find(_attachmentPath, &preview)) {
			preview = QPixmap(_attachmentPath);
			QPixmapCache::insert(_attachmentPath, preview);
		}
	}
	else if (_attachmentType.contains("video")) {
		if (!QPixmapCache::find(_attachmentPath, &preview)) {
			preview = QPixmap(_attachmentPreviewSize); 
			QPixmapCache::insert(_attachmentPath, preview);
		}
		preview.fill(Qt::gray);
	}

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

			painter.setPen(Qt::white);
			QFont font("Arial", 16);

			const auto attachmentsCountText = "+ " + QString::number(_parentMessage->attachmentsLength() - 1);
			const auto attachmentsCountTextSize = textSize(attachmentsCountText, font);

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
	if (_attachmentType.contains("image"))
		_attachmentPreviewSize = getMinimumSizeWithAspectRatio(QPixmap(_attachmentPath).size(), _attachmentWidth);
	else if (_attachmentType.contains("video"))
		_attachmentPreviewSize = getMinimumSizeWithAspectRatio(QSize(100, 100), _attachmentWidth);
}