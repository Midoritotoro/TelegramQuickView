#include "ScrollArea.h"
#include <QScrollBar>


ScrollArea::ScrollArea(QWidget* parent):
	QScrollArea(parent)
{

}

void ScrollArea::scrollHandler() {
	if (verticalScrollBar()->maximum() == verticalScrollBar()->value()) {
		// if (...) - ����� �������� �� ������� �����, ������� ����� ����������
		emit addContentsNeeded();
	}
}