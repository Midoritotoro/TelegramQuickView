#pragma once

#include "MessageWidget.h"
#include <QVBoxLayout>

#include "History.h"
#include <memory>

#include "ContinuousScroll.h"

class TelegramParser;
class MessageMediaViewer;
class MouseDetector;


class TelegramPostQuickView: public QWidget {
	Q_OBJECT
private:
	MessageWidget::MessageMediaDisplayMode _displayMode;

	QVBoxLayout* _chatScrollAreaLayout = nullptr;

	std::unique_ptr<MessageMediaViewer> _messageMediaViewer = nullptr;
	std::unique_ptr<TelegramParser> _telegramParser = nullptr;

	std::unique_ptr<MouseDetector> _mouseDetector = nullptr;
	std::shared_ptr<History> _messagesHistory;

	ContinuousScroll* _chatScrollArea = nullptr;

	int _currentPostIndex;
public:
	TelegramPostQuickView(QWidget* parent = nullptr);

	void makeMessage(
		const QString& messageText = "",
		const QStringList& attachmentsPaths = {});
	void setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode displayMode);
public Q_SLOTS:
	void attachmentCliked();
protected:
	void addContent();
};