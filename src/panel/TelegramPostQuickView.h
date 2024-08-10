#pragma once

#include <QScrollArea>

#include "MessageWidget.h"
#include "MessageMediaViewer.h"
#include "History.h"


class History;


class TelegramPostQuickView: public QWidget {
private:
	Q_OBJECT
	int _panelWidth;
	QGridLayout* _chatScrollAreaLayout = nullptr;
	MessageMediaViewer* _messageMediaViewer = nullptr;
	MessagesList _messagesList;
	History* _messagesHistory = nullptr;
public:
	TelegramPostQuickView(QWidget* parent = nullptr);

	void makeMessage(const QString& author, const QString& messageText = "", const QUrlList& attachmentsPaths = {});

	[[nodiscard]] const MessagesList& messages() const;

	[[nodiscard]] int indexOfMessage(MessageWidget* messageWidget);
	[[nodiscard]] MessageWidget* messageAt(int index);
public Q_SLOTS:
	void attachmentCliked();
};