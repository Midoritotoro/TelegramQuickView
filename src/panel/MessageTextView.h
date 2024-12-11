#pragma once

#include <QWidget>
#include <QLabel>


class MessageTextView : public QWidget {
	Q_OBJECT
private:
	QLabel* _textLabel = nullptr;
public:
	MessageTextView(QWidget* parent = nullptr);

	void setText(const QString& text);
protected:
	void paintEvent(QPaintEvent* event) override;
};