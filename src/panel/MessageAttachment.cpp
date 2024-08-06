#include "MessageAttachment.h"


MessageAttachment::MessageAttachment(QString attachmentPath, int attachmentWidth, QWidget* parent) :
	ClickableLabel(parent), _attachmentPath(attachmentPath), _attachmentType(MediaPlayer::detectMediaType(attachmentPath))
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
}

QSize MessageAttachment::getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
	return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
}

QString MessageAttachment::attachmentPath() const {
	return _attachmentPath;
}

QString MessageAttachment::attachmentType() const {
	return _attachmentType;
}