#include "ScrollArea.h"
#include <QScrollBar>


ScrollArea::ScrollArea(QWidget* parent):
	QScrollArea(parent)
{
	
}

void ScrollArea::scrollHandler() {
	if (verticalScrollBar()->maximum() == verticalScrollBar()->value()) {
		// if (...) - Нужна проверка на наличие медиа, которые можно подгрузить
		emit addContentsNeeded();
	}
}
