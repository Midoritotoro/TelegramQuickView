#pragma once

#include "../core/Types.h"
#include "../ui/layouts/MosaicLayout.h"

#include <QVBoxLayout>

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

	void setMediaDisplayMode(MediaDisplayMode displayMode);
	[[nodiscard]] MediaDisplayMode mediaDisplayMode() const noexcept;

	void setText(const QString& text);
	[[nodiscard]] QString text() const noexcept;

	void setAttachments(const QStringList& attachments);
	[[nodiscard]] const MessageAttachmentsList& attachments() const noexcept;

	[[nodiscard]] int indexOfAttachment(not_null<MessageAttachment*> messageAttachment) const noexcept;
	[[nodiscard]] MessageAttachment* attachmentAt(int index) const noexcept;
	[[nodiscard]] int attachmentsLength() const noexcept;

	[[nodiscard]] bool hasAttachments() const noexcept;
	[[nodiscard]] bool hasText() const noexcept;
private:
	[[nodiscard]] int attachmentsHeight() const noexcept;

	MessageAttachmentsList _attachments;

	QVBoxLayout* _messageLayout = nullptr;
	MediaDisplayMode _mediaDisplayMode;

	FlatLabel* _textLabel = nullptr;

	Fn<void(const QSize&)> _recountSizeCallback = nullptr;
};