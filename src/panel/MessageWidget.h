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
	inline void setMessageMediaDisplayMode(MessageMediaDisplayMode displayMode) { _mediaDisplayMode = displayMode; }

	[[nodiscard]] inline MessageMediaDisplayMode messsageMediaDisplayMode() const noexcept { return _mediaDisplayMode; }
	[[nodiscard]] inline QString messageText() const noexcept { return _telegramMessage->text; }
	[[nodiscard]] inline const MessageAttachmentsList& messageAttachments() const noexcept { return _telegramMessage->attachments; }

	[[nodiscard]] inline int indexOfAttachment(MessageAttachment* messageAttachment) const noexcept { return _telegramMessage->attachments.indexOf(messageAttachment); }
	[[nodiscard]] MessageAttachment* attachmentAt(int index) const noexcept;
	[[nodiscard]] inline int attachmentsLength() const noexcept { return _telegramMessage->attachments.length(); }

	[[nodiscard]] inline bool hasAttachments() const noexcept { return !_telegramMessage->attachments.isEmpty(); }
	[[nodiscard]] inline bool hasText() const noexcept { return !_telegramMessage->text.isEmpty(); }
private:
	std::shared_ptr<TelegramMessage> _telegramMessage = nullptr;
	QGridLayout* _messageLayout = nullptr;
	MessageMediaDisplayMode _mediaDisplayMode;
};