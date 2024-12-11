#pragma once

#include <QList>

class MessageWidget;
typedef QList<MessageWidget*> MessagesList;


class History {
private:
	MessagesList _messagesList;
public:
	void makeMessage(not_null<MessageWidget*> messageWidget);

	[[nodiscard]] int indexOfMessage(not_null<MessageWidget*> message) const noexcept;

	[[nodiscard]] MessageWidget* messageAt(int index) const noexcept;

	[[nodiscard]] int messagesWithAttachmentsCount() const noexcept;
	[[nodiscard]] int count() const noexcept;
};