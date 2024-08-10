#pragma once

#include <QList>

class MessageWidget;
typedef QList<MessageWidget*> MessagesList;


class History {
private:
	MessagesList _messagesList;
public:
	void makeMessage(MessageWidget* messageWidget);

	[[nodiscard]] int indexOfMessage(MessageWidget* message) const noexcept;
	[[nodiscard]] MessageWidget* messagetAt(int index) const noexcept;
	[[nodiscard]] int messagesLength() const noexcept;
};