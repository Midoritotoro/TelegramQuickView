#pragma once

#include <QScrollArea>
#include <QWidget>


class ScrollArea : public QScrollArea {
private:
	Q_OBJECT
public:
	ScrollArea(QWidget* parent = nullptr);

Q_SIGNALS:
	void addContentsNeeded();
public:
	void scrollHandler();
};