#pragma once

#include <QWidget>
#include <QGridLayout>


class MessageMediaViewer: public QWidget {
private:
	Q_OBJECT
	QGridLayout* _grid = nullptr;
public:
	MessageMediaViewer(QWidget* parent = nullptr);

	void toNext();
	void toPrevious();
};