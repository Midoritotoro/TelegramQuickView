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

#include "../core/CoreUtility.h"


#ifdef min
#undef min
#endif min

namespace {
	inline constexpr auto kInactiveTimeout = 2000;

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
	const QString& attachmentPath,
	bool display
)
	: QAbstractButton(parentMessage)
	, _attachmentPath(attachmentPath)
	, _attachmentType(Media::detectMediaType(attachmentPath))
	, _parentMessage(parentMessage)
	, _display(display)
{
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	setMouseTracking(true);
	setCursor(Qt::PointingHandCursor);

	if (display == false)
		return;

	const auto imageSize = Media::MediaResolution(_attachmentPath, _attachmentType);

	_previewSize =
		countAttachmentSize(core::utility::GetMinimumSizeWithAspectRatio(imageSize, style::maximumMessageWidth),
			style::maximumMessageWidth, style::maximumMessageWidth);
	setFixedSize(_previewSize);

	concurrent::invokeAsync([this] {
		Media::GenerateThumbnail(_attachmentPath, size());
		update();
	});

	qDebug() << "MessageAttachment::MessageAttachment";
}

QSize MessageAttachment::sizeHint() const {
	return _previewSize;
}

QSize MessageAttachment::minimumSizeHint() const {
	return _previewSize;
}

QString MessageAttachment::attachmentPath() const noexcept {
	return _attachmentPath;
}

Media::Type MessageAttachment::attachmentType() const noexcept {
	return _attachmentType;
}

void MessageAttachment::setParentMessage(not_null<Message*> parentMessage) {
	_parentMessage = parentMessage;
}

not_null<Message*> MessageAttachment::parentMessage() const noexcept {
	return _parentMessage;
}

void MessageAttachment::setDisplay(bool display) {
	_display = display;
}

bool MessageAttachment::display() const noexcept {
	return _display;
}

void MessageAttachment::paintEvent(QPaintEvent* event) {
	if (_display == false)
		return;

	const auto preview = Media::FindPreviewInCache(_attachmentPath);
	if (preview.isNull())
		return;

	auto painter = QPainter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

	switch (_parentMessage->mediaDisplayMode()) {
	case Message::MediaDisplayMode::Stack:
		if (_parentMessage->attachmentsLength() == 1 && _parentMessage->hasText() == false)
			style::RoundCorners(painter, size(), 15);
		else if (_parentMessage->attachmentsLength() > 1 && _parentMessage->indexOfAttachment(this) == 0)
			style::RoundTopCorners(painter, size(), 15);

		painter.drawPixmap(0, 0, preview);
		break;

	case Message::MediaDisplayMode::PreviewWithCount:
		_parentMessage->hasText()
			? style::RoundTopCorners(painter, size(), 15)
			: style::RoundCorners(painter, size(), 15);

		if (_parentMessage->attachmentsLength() > 1) {
			painter.drawPixmap(0, 0, preview);

			painter.setOpacity(0.4);
			painter.fillRect(rect(), Qt::black);

			paintAttachmentCount(painter);
		}
		else
			painter.drawPixmap(0, 0, preview);

		break;
	}
}

void MessageAttachment::paintAttachmentCount(QPainter& painter) {
	const auto font = style::font(13, 0, 0);

	const auto attachmentsCountText = "+" + QString::number(_parentMessage->attachmentsLength() - 1);
	const auto attachmentsCountTextSize = core::utility::TextSize(attachmentsCountText, font);

	auto attachmentsCountTextRect = QRect(QPoint(), attachmentsCountTextSize);
	attachmentsCountTextRect.moveCenter(rect().center());

	painter.setPen(Qt::white);
	painter.setFont(font);

	painter.setOpacity(1.0);
	painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, attachmentsCountText);
}

void MessageAttachment::mouseMoveEvent(QMouseEvent* event) {
	if (_thumbnailQuality != Media::Quality::Ultra) {
		if (_touchTimer == nullptr)
			_touchTimer = new core::Timer();

		_touchTimer->setCallback([this] {
			concurrent::invokeAsync([this] {
				QPixmapCache::remove(_attachmentPath);
				Media::GenerateThumbnail(
					_attachmentPath, size(), Media::Quality::Ultra);

				update();
				_touchTimer = nullptr;
			});
		});

		_touchTimer->callOnce(kInactiveTimeout);
		_thumbnailQuality = Media::Quality::Ultra;
	}
}

void MessageAttachment::mousePressEvent(QMouseEvent* event) {
	QAbstractButton::mousePressEvent(event);
}

bool MessageAttachment::event(QEvent* _event) {
	return QAbstractButton::event(_event);
}