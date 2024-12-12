#pragma once

#include "../media/ClickableLabel.h"
#include "../core/Types.h"

#include <QPixmap>

class MessageWidget;
class QPaintEvent;
class QResizeEvent;


class MessageAttachment final: public ClickableLabel {
	Q_OBJECT
public:
	enum class AttachmentType {
		Audio,
		Video,
		Photo,
		Unknown
	};

	MessageAttachment(
		not_null<MessageWidget*> parentMessage,
		const QString& attachmentPath);

	void setParentMessage(not_null<MessageWidget*> parentMessage);

	[[nodiscard]] QString attachmentPath() const noexcept;
	[[nodiscard]] AttachmentType attachmentType() const noexcept;

	[[nodiscard]] MessageWidget* parentMessage() const noexcept;
	[[nodiscard]] static AttachmentType detectMediaType(const QString& filePath) noexcept;
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void paintAttachmentCount(QPainter& painter);
	void updateSize();

	QString _attachmentPath;
	AttachmentType _attachmentType;

	MessageWidget* _parentMessage = nullptr;
};