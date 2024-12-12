#pragma once

#include "../core/Types.h"
#include <QList>

class Message;
typedef QList<Message*> MessagesList;


class History {
private:
	MessagesList _messagesList;
public:
	void makeMessage(not_null<Message*> messageWidget);

	[[nodiscard]] int indexOfMessage(not_null<Message*> message) const noexcept;

	[[nodiscard]] Message* messageAt(int index) const noexcept;

	[[nodiscard]] int messagesWithAttachmentsCount() const noexcept;
	[[nodiscard]] int count() const noexcept;
};