#pragma once

#include <QAbstractButton>
#include "../core/Types.h"

#include <QPixmap>

class Message;
class QPaintEvent;
class QResizeEvent;


class MessageAttachment final: public QAbstractButton {
	Q_OBJECT
public:
	enum class AttachmentType {
		Audio,
		Video,
		Photo,
		Unknown
	};

	MessageAttachment(
		not_null<Message*> parentMessage,
		const QString& attachmentPath);

	void setParentMessage(not_null<Message*> parentMessage);

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] AttachmentType attachmentType() const noexcept;

	[[nodiscard]] Message* parentMessage() const noexcept;
	[[nodiscard]] static AttachmentType detectMediaType(const QString& filePath) noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void paintAttachmentCount(QPainter& painter);
	void updateSize();

	QString _attachmentPath;
	AttachmentType _attachmentType;

	Message* _parentMessage = nullptr;
};