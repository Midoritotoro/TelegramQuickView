#pragma once

#include "MessageAttachment.h"

typedef QList<MessageAttachment*> MessageAttachmentsList;
class QGridLayout;

typedef struct _TelegramMessage {
	QString date;
	QString author;
	QString text;
	MessageAttachmentsList attachments;
} TelegramMessage;


class MessageWidget: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _messageLayout = nullptr;
	TelegramMessage* _telegramMessage = nullptr;
public:
	MessageWidget(const QString& author, QWidget* parent = nullptr);

	void addMessageText(const QString& text);
	void addMessageAttachments(const QUrlList& attachmentsPaths, int maximumMessageWidth);

	[[nodiscard]] QString messageText() const noexcept;
	[[nodiscard]] const MessageAttachmentsList& messageAttachments() const noexcept;

	[[nodiscard]] int indexOfAttachment(MessageAttachment* messageAttachment) const noexcept;
	[[nodiscard]] MessageAttachment* attachmentAt(int index) const noexcept;
	[[nodiscard]] int attachmentsLength() const noexcept;
};