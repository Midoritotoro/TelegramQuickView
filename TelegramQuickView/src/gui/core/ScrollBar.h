#pragma once

#include <QtWidgets>
#include <QtCore>

#include <windows.h>

class ScrollBar: public QMainWindow
{
private:
	Q_OBJECT
public:
	ScrollBar(QWidget* parent = nullptr);
	void CreateScrollBar();
};
