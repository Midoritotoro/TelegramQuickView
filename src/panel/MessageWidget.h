#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QUrl>
#include <memory>


class MessageAttachment;

typedef QList<MessageAttachment*> MessageAttachmentsList;


class MessageWidget: public QWidget {
	Q_OBJECT
public:
	enum class MessageMediaDisplayMode {
		Album,
		Stack,
		PreviewWithCount
	};

	MessageWidget(QWidget* parent = nullptr);

	void addMessageText(const QString& text);
	void addMessageAttachments(const QStringList& attachmentsPaths);

	void setMessageMediaDisplayMode(MessageMediaDisplayMode displayMode);

	[[nodiscard]] MessageMediaDisplayMode messsageMediaDisplayMode() const noexcept;

	[[nodiscard]] QString messageText() const noexcept;
	[[nodiscard]] const MessageAttachmentsList& messageAttachments() const noexcept;

	[[nodiscard]] int indexOfAttachment(MessageAttachment* messageAttachment) const noexcept;
	[[nodiscard]] MessageAttachment* attachmentAt(int index) const noexcept;
	[[nodiscard]] int attachmentsLength() const noexcept;

	[[nodiscard]] bool hasAttachments() const noexcept;
	[[nodiscard]] bool hasText() const noexcept;
private:
	QString _text;
	MessageAttachmentsList _attachments;
	QGridLayout* _messageLayout = nullptr;
	MessageMediaDisplayMode _mediaDisplayMode;
};