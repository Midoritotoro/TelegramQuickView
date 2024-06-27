#pragma once

#include <QtWidgets>
#include <QtCore>

#include <atomic>

#include <windows.h>

class MessagesArea : public QWidget
{
private:
	Q_OBJECT
		QListView* _ChatView = nullptr;
	QStandardItemModel* _ChatModel = nullptr;
public:
	MessagesArea(QWidget* parent = nullptr);
	void addMessage(const QString& text);
};

class ScrollBar : public QMainWindow
{
private:
	Q_OBJECT
		QWidget* chatListWidget = nullptr;
	QGridLayout* chatGridLayout = nullptr;
	QScrollArea* chatScrollArea = nullptr, * messageScrollArea = nullptr;
	QListWidget* chatList = nullptr;
public:
	ScrollBar(QWidget* parent = nullptr);
	void CreateScrollBar();
	void adjustTextWidth(QLabel* label);
	void createChat(const QString imagePath, const QString chatName);
public slots:
	void on_channelLabelClicked();
};
