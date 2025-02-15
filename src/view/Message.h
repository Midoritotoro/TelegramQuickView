#pragma once

#include "../core/Types.h"

#include <QWidget>
#include <QList>


class MessageAttachment;
class FlatLabel;

typedef QList<MessageAttachment*> MessageAttachmentsList;


class Message: public QWidget {
	Q_OBJECT
public:
	enum class MediaDisplayMode {
		Album,
		Stack,
		PreviewWithCount
	};

	Message(QWidget* parent = nullptr);

	[[nodiscard]] QSize sizeHint() const override;
	[[nodiscard]] QSize minimumSizeHint() const override;

	void setMediaDisplayMode(MediaDisplayMode displayMode);
	[[nodiscard]] MediaDisplayMode mediaDisplayMode() const noexcept;

	void setAttributes(
		const QString& text,
		const QStringList& attachments);

	[[nodiscard]] QString text() const noexcept;
	[[nodiscard]] const MessageAttachmentsList& attachments() const noexcept;

	[[nodiscard]] int indexOfAttachment(not_null<MessageAttachment*> messageAttachment) const noexcept;
	[[nodiscard]] MessageAttachment* attachmentAt(int index) const noexcept;
	[[nodiscard]] int attachmentsLength() const noexcept;

	[[nodiscard]] bool hasAttachments() const noexcept;
	[[nodiscard]] bool hasText() const noexcept;
protected:
	void resizeEvent(QResizeEvent* event) override;
private:
	void updateGeometry();

	[[nodiscard]] int attachmentsHeight() const noexcept;
	[[nodiscard]] int maximumAttachmentsWidth() const noexcept;

	MessageAttachmentsList _attachments;
	MediaDisplayMode _mediaDisplayMode;

	FlatLabel* _textLabel = nullptr;
};