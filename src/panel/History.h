#pragma once

#include <QList>

class MessageWidget;
typedef QList<MessageWidget*> MessagesList;


class History {
private:
	MessagesList _messagesList;
public:
	void makeMessage(MessageWidget* messageWidget);

	[[nodiscard]] inline int indexOfMessage(MessageWidget* message) const noexcept { return _messagesList.indexOf(message); }
	[[nodiscard]] MessageWidget* messageAt(int index) const noexcept;

	[[nodiscard]] int messagesWithAttachmentsCount() const noexcept;
	[[nodiscard]] inline int count() const noexcept { return _messagesList.length(); }
};