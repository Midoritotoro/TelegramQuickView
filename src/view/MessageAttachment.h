#pragma once

#include "../core/Types.h"
#include "../media/MediaCommon.h"

#include <QAbstractButton>

class Message;
class QPaintEvent;
class QResizeEvent;


class MessageAttachment final: public QAbstractButton {
	Q_OBJECT
public:
	[[nodiscard]] QSize sizeHint() const override;
	[[nodiscard]] QSize minimumSizeHint() const override;

	MessageAttachment(
		not_null<Message*> parentMessage,
		const QString& attachmentPath,
		bool display = true);

	void setParentMessage(not_null<Message*> parentMessage);

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] Media::Type attachmentType() const noexcept;

	[[nodiscard]] Message* parentMessage() const noexcept;
	[[nodiscard]] bool display() const noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void paintAttachmentCount(QPainter& painter);

	QString _attachmentPath;
	Media::Type _attachmentType;

	QSize _previewSize;
	Message* _parentMessage = nullptr;

	bool _display;
};