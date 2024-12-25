#include "MessageAttachment.h"
#include "Message.h"

#include <QMimeDatabase>
#include <QPainter>

#include "../ui/style/StyleCore.h"
#include "../core/Time.h"

#include "../ui/style/StyleFont.h"
#include <QPaintEvent>

#include "../ui/style/StyleWidgets.h"
#include "../core/CoreConcurrent.h"

#ifdef min
#undef min
#endif min

namespace {
	[[nodiscard]] QSize countAttachmentSize(
		const QSize& originalSize,
		int maxWidth,
		int maxHeight)
	{
		if (originalSize.isEmpty() || maxWidth <= 0 || maxHeight <= 0)
			return QSize();

		double originalWidth = originalSize.width();
		double originalHeight = originalSize.height();

		double widthScale = (double)maxWidth / originalWidth;
		double heightScale = (double)maxHeight / originalHeight;

		double scaleFactor = std::min(widthScale, heightScale);

		int newWidth = static_cast<int>(originalWidth * scaleFactor);
		int newHeight = static_cast<int>(originalHeight * scaleFactor);

		return QSize(newWidth, newHeight);
	}
}


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

	setFixedSize(size());

	concurrent::on_main([this] {
		const auto imageSize = style::MediaPreview(_attachmentPath).size();

		setFixedSize(
			countAttachmentSize(style::getMinimumSizeWithAspectRatio(imageSize, style::maximumMessageWidth),
				style::maximumMessageWidth, style::maximumMessageWidth)
		);

		qDebug() << "countedSize: " << size();

		style::GenerateThumbnail(_attachmentPath, size());
		update();
	});
}

QSize MessageAttachment::sizeHint() const {
	return size();
}

QSize MessageAttachment::minimumSizeHint() const {
	return size();
}

void MessageAttachment::paintEvent(QPaintEvent* event) {
	//static int time;
	//const auto ms = Time::now();
	//const auto timer = gsl::finally([=] { /*qDebug() << "MessageAttachment::paintEvent: " << Time::now() - ms << " ms";*/ time += Time::now() - ms; qDebug() <<
	//	"totaltime: " << time;  });
	//

	if (style::FindPreviewInCache(_attachmentPath) == false)
		return;

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

void MessageAttachment::paintAttachmentCount(QPainter& painter) {
	const auto font = style::font(13, 0, 0);

	const auto attachmentsCountText = "+" + QString::number(_parentMessage->attachmentsLength() - 1);
	const auto attachmentsCountTextSize = style::TextSize(attachmentsCountText, font);

	QRect attachmentsCountTextRect(QPoint(), attachmentsCountTextSize);
	attachmentsCountTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font);

	painter.setOpacity(1.0);
	painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, attachmentsCountText);
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