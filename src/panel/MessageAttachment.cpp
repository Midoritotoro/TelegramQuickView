#include "MessageAttachment.h"

#include "MessageWidget.h"


#include <QMimeDatabase>
#include <QUrl>


MessageAttachment::MessageAttachment(QString attachmentPath, int attachmentWidth, QWidget* parent)
: ClickableLabel(parent)
, _attachmentPath(attachmentPath)
, _attachmentType(detectMediaType(attachmentPath))
{
	setMouseTracking(true);
	setScaledContents(true);
	setBackgroundRole(QPalette::Dark);
	setCursor(Qt::PointingHandCursor);

	if (_attachmentType.contains("image")) {
		QImage attachmentImage(attachmentPath);
		attachmentImage = attachmentImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		QSize size = getMinimumSizeWithAspectRatio(attachmentImage.size(), attachmentWidth);
		attachmentImage = attachmentImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		setPixmap(QPixmap::fromImage(attachmentImage));
	}
	else if (_attachmentType.contains("video")) {
		QImage attachmentImage(attachmentWidth / 2, attachmentWidth / 2, QImage::Format_ARGB32_Premultiplied);
		attachmentImage.fill(Qt::white);
		QSize size = getMinimumSizeWithAspectRatio(attachmentImage.size(), attachmentWidth);
		attachmentImage = attachmentImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		setPixmap(QPixmap::fromImage(attachmentImage));
	}
}

QSize MessageAttachment::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

void MessageAttachment::setParentMessage(MessageWidget* messageWidget) {
	_parentMessage = messageWidget;
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