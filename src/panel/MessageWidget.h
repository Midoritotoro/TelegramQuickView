#pragma once

#include <QWidget>
#include <QLabel>

#include "MessageAttachment.h"


class MessageWidget: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _messageLayout = nullptr;
	QString _messageText;
	QUrlList _attachmentsPaths;
	MessageAttachment* _messageAttachment = nullptr;
public:
	MessageWidget(QWidget* parent = nullptr);

	void addMessageText(const QString& text);
	void addMessageAttachments(const QUrlList& attachmentsPaths, int maximumMessageWidth);

	[[nodiscard]] QString getMessageText() const;
	[[nodiscard]] QUrlList getMessageAttachmentsPaths() const;
	[[nodiscard]] MessageAttachment* getMessageAttachment() const;
};