#pragma once

#include "../core/Types.h"
#include "../core/Timer.h"

#include "../media/MediaCommon.h"

#include <QAbstractButton>

class Message;
class QPaintEvent;
class QResizeEvent;


class MessageAttachment final: public QAbstractButton {
	Q_OBJECT
public:
	MessageAttachment(
		not_null<Message*> parentMessage,
		const QString& attachmentPath,
		bool display = true);

	[[nodiscard]] QSize sizeHint() const override;
	[[nodiscard]] QSize minimumSizeHint() const override;

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] Media::Type attachmentType() const noexcept;

	void setParentMessage(not_null<Message*> parentMessage);
	[[nodiscard]] not_null<Message*> parentMessage() const noexcept;

	void setDisplay(bool display);
	[[nodiscard]] bool display() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

	bool event(QEvent* event) override;
private:
	void paintAttachmentCount(QPainter& painter);

	QString _attachmentPath;
	Media::Type _attachmentType;

	QSize _previewSize;
	not_null<Message*> _parentMessage;

	bool _display;

	core::Timer* _touchTimer = nullptr;
	Media::Quality _thumbnailQuality = Media::Quality::Medium;
};