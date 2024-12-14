#pragma once

#include "Message.h"
#include <QVBoxLayout>

#include "History.h"
#include <memory>

#include "../ui/widgets/ContinuousScroll.h"
#include "MediaViewer.h"

#include "../core/MouseDetector.h"
#include "../parser/TelegramParser.h"


class MessagesView: public QWidget {
	Q_OBJECT
private:
	Message::MediaDisplayMode _displayMode;

	QVBoxLayout* _chatScrollAreaLayout = nullptr;

	std::unique_ptr<MediaViewer> _messageMediaViewer = nullptr;
	std::unique_ptr<TelegramParser> _telegramParser = nullptr;

	std::unique_ptr<MouseDetector> _mouseDetector = nullptr;
	std::shared_ptr<History> _messagesHistory;

	ContinuousScroll* _chatScrollArea = nullptr;

	int _currentPostIndex;
public:
	MessagesView(QWidget* parent = nullptr);

	void makeMessage(
		const QString& messageText = "",
		const QStringList& attachmentsPaths = {});
	void setMessageMediaDisplayMode(Message::MediaDisplayMode displayMode);
public Q_SLOTS:
	void attachmentCliked();
protected:
	void addContent();
};