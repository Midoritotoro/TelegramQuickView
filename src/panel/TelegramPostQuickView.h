#pragma once

#include "MessageWidget.h"
#include <QVBoxLayout>
#include "History.h"
#include <memory>

class MessageMediaViewer;

class QScrollArea;
class QListView;


class TelegramPostQuickView: public QWidget {
	Q_OBJECT
private:
	int _panelWidth;
	MessageWidget::MessageMediaDisplayMode _displayMode;

	QVBoxLayout* _chatScrollAreaLayout = nullptr;
	std::unique_ptr<MessageMediaViewer> _messageMediaViewer = nullptr;
	QListView* _chatScrollArea = nullptr;

	std::shared_ptr<History> _messagesHistory;
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