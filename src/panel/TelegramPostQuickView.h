#pragma once

#include <QScrollArea>

#include "MessageWidget.h"
#include "MessageMediaViewer.h"

typedef QList<MessageWidget*> MessagesList;


class TelegramPostQuickView: public QWidget {
private:
	Q_OBJECT
	int panelWidth;
	QScrollArea* _chatScrollArea = nullptr;
	QGridLayout* _chatScrollAreaLayout = nullptr;
	QGridLayout* _grid = nullptr;
	QWidget* _chatScrollAreaWidget = nullptr;
	MessageMediaViewer* _messageMediaViewer = nullptr;
	MessagesList _messagesList;
public:
	TelegramPostQuickView(const QString& messageText = "", const QUrlList& attachmentsPaths = {}, QWidget* parent = nullptr);

	void addMessage(const QString& messageText, const QUrlList& attachmentsPaths = {});

	[[nodiscard]] const MessagesList& messages() const;

	[[nodiscard]] int indexOfMessage(MessageWidget* messageWidget);
	[[nodiscard]] MessageWidget* messagetAt(int index);
public Q_SLOTS:
	void attachmentCliked();
};