#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QUrl>
#include <memory>


class MessageAttachment;

typedef QList<QUrl> QUrlList;
typedef QList<MessageAttachment*> MessageAttachmentsList;

typedef struct _TelegramMessage {
	QString text;
	MessageAttachmentsList attachments;
} TelegramMessage;


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
	void addMessageAttachments(const QUrlList& attachmentsPaths, int maximumMessageWidth);

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
	std::unique_ptr<TelegramMessage> _telegramMessage = nullptr;
	QGridLayout* _messageLayout = nullptr;
	MessageMediaDisplayMode _mediaDisplayMode;
};