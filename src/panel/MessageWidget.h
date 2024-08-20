#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QUrl>

class MessageAttachment;

typedef QList<QUrl> QUrlList;
typedef QList<MessageAttachment*> MessageAttachmentsList;

typedef struct _TelegramMessage {
	QString date;
	QString author;
	QString text;
	MessageAttachmentsList attachments;
} TelegramMessage;


class MessageWidget: public QWidget {
	Q_OBJECT
public:
	enum class MessageMediaDisplayMode {
		Stack,
		PreviewWithCount
	};

	MessageWidget(const QString& author, QWidget* parent = nullptr);

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
	QGridLayout* _messageLayout = nullptr;
	TelegramMessage* _telegramMessage = nullptr;
	MessageMediaDisplayMode _mediaDisplayMode;
};