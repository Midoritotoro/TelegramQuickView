#pragma once

#include <QWidget>
#include <QLabel>


class MessageTextView : public QWidget {
private:
	Q_OBJECT
	QLabel* _textLabel = nullptr;
public:
	MessageTextView(QWidget* parent = nullptr);
	void setText(const QString& text);

};