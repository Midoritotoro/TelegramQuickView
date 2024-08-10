#include "History.h"
#include "MessageWidget.h"


void History::makeMessage(MessageWidget* messageWidget) {

}

int History::indexOfMessage(MessageWidget* message) const noexcept {
	return _messagesList.indexOf(message);
}

MessageWidget* History::messagetAt(int index) const noexcept {
	return _messagesList.at(index);
}

int History::messagesLength() const noexcept {
	return _messagesList.length();
}
