#pragma once

#include <QWidget>
#include <QLabel>
#include <QGridLayout>


class MessageWidget : public QWidget {
private:
	Q_OBJECT
public:
	MessageWidget(QWidget* parent = nullptr);
};