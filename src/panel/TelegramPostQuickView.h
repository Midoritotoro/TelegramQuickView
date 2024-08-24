#pragma once

#include "MessageWidget.h"
#include <QVBoxLayout>


class History;
class MessageMediaViewer;
class ScrollArea;

class TelegramPostQuickView: public QWidget {
	Q_OBJECT
private:
	int _panelWidth;
	QVBoxLayout* _chatScrollAreaLayout = nullptr;
	MessageMediaViewer* _messageMediaViewer = nullptr;
	History* _messagesHistory = nullptr;
	ScrollArea* _chatScrollArea = nullptr;
	MessageWidget::MessageMediaDisplayMode _displayMode;
public:
	TelegramPostQuickView(QWidget* parent = nullptr);

	void makeMessage(const QString& messageText = "", const QUrlList& attachmentsPaths = {});
	void setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode displayMode);
public Q_SLOTS:
	void attachmentCliked();
protected:
	void showEvent(QShowEvent* event) override;
	void addContentsRequest();
};