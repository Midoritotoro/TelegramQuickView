#include "History.h"
#include "MessageWidget.h"


void History::makeMessage(MessageWidget* messageWidget) {
	return _messagesList.append(messageWidget);
}

int History::indexOfMessage(MessageWidget* message) const noexcept {
	return _messagesList.indexOf(message);
}

MessageWidget* History::messageAt(int index) const noexcept {
	if (count() > 0)
		return _messagesList.at(index);
	return nullptr;
}

int History::messagesWithAttachmentsCount() const noexcept {
	int messagesWithAttachmentsCount = 0;

	for (int index = 0; index < count(); ++index)
		if (messageAt(index)->hasAttachments())
			++messagesWithAttachmentsCount;

	return messagesWithAttachmentsCount;
}

int History::count() const noexcept {
	return _messagesList.length();
}
