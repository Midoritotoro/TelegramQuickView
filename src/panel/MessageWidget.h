#pragma once

#include <QWidget>
#include <QLabel>

#include "MessageAttachment.h"

typedef QList<MessageAttachment*> MessageAttachmentsList;


class MessageWidget: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _messageLayout = nullptr;
	QString _messageText;
	MessageAttachmentsList _messageAttachmentsList;
public:
	MessageWidget(QWidget* parent = nullptr);

	void addMessageText(const QString& text);
	void addMessageAttachments(const QUrlList& attachmentsPaths, int maximumMessageWidth);

	[[nodiscard]] QString getMessageText() const;
	[[nodiscard]] const MessageAttachmentsList& getMessageAttachments() const;

	[[nodiscard]] int indexOfAttachment(MessageAttachment* messageAttachment);
	[[nodiscard]] MessageAttachment* attachmentAt(int index);
};