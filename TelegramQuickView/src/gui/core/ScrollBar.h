#pragma once

#include <QtWidgets>
#include <QtCore>

#include <windows.h>

class ScrollBar: public QMainWindow
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
};
