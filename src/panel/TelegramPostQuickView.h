#pragma once

#include <QScrollArea>

#include "MessageWidget.h"
#include "MessageMediaViewer.h"
#include "History.h"
#include "ScrollArea.h"

#include <QVBoxLayout>


class History;


class TelegramPostQuickView: public QWidget {
	Q_OBJECT
public:
	TelegramPostQuickView(QWidget* parent = nullptr);

	void makeMessage(const QString& author, const QString& messageText = "", const QUrlList& attachmentsPaths = {});
	void setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode displayMode);

	[[nodiscard]] const MessagesList& messages() const;

	[[nodiscard]] int indexOfMessage(MessageWidget* messageWidget);
	[[nodiscard]] MessageWidget* messageAt(int index);
public Q_SLOTS:
	void attachmentCliked();
protected:
	void showEvent(QShowEvent* event) override;
	void addContentsRequest();
private:
	int _panelWidth;
	QVBoxLayout* _chatScrollAreaLayout = nullptr;
	MessageMediaViewer* _messageMediaViewer = nullptr;
	MessagesList _messagesList;
	History* _messagesHistory = nullptr;
	ScrollArea* _chatScrollArea = nullptr;
	MessageWidget::MessageMediaDisplayMode _displayMode;
};