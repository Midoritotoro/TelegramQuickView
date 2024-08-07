#pragma once

#include <QWidget>
#include <QLabel>

#include "MessageAttachment.h"


class MessageWidget: public QWidget {
private:
	Q_OBJECT
	QString _messageText;
	QString _attachmentPath;
public:
	MessageWidget(QWidget* parent = nullptr);

	[[nodiscard]] QWidget* createMessageWidget();
	[[nodiscard]] QLabel* createMessageTextLabel();

	[[nodiscard]] QString& const getMessageText();
	[[nodiscard]] QString& const getMessageAttachmentsPath();
};