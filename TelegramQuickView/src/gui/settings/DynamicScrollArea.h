#pragma once

#include <qscrollarea.h>
#include <qframe.h>
#include <windows.h>

class DynamicScrollArea : public QScrollArea
{
private:
	Q_OBJECT
public:
	DynamicScrollArea(QWidget* parent = nullptr);
	~DynamicScrollArea();
};
