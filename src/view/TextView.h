#pragma once

#include <QWidget>
#include <QLabel>


class TextView : public QWidget {
	Q_OBJECT
private:
	QLabel* _textLabel = nullptr;
public:
	TextView(QWidget* parent = nullptr);

	void setText(const QString& text);
protected:
	void paintEvent(QPaintEvent* event) override;
};