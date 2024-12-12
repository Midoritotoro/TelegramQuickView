#include "History.h"
#include "Message.h"


void History::makeMessage(not_null<Message*> messageWidget) {
	return _messagesList.append(messageWidget);
}

int History::indexOfMessage(not_null<Message*> message) const noexcept {
	return _messagesList.indexOf(message); 
}

Message* History::messageAt(int index) const noexcept {
	if (count() > 0)
		return _messagesList.at(index);
	return nullptr;
}

int History::messagesWithAttachmentsCount() const noexcept {
	auto messagesWithAttachmentsCount = 0;

	for (int index = 0; index < count(); ++index)
		if (messageAt(index)->hasAttachments())
			++messagesWithAttachmentsCount;

	return messagesWithAttachmentsCount;
}

int History::count() const noexcept {
	return _messagesList.length();
}

