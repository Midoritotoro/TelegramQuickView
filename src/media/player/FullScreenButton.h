#pragma once

#include <QPushButton>


class FullScreenButton : public QPushButton {
	Q_OBJECT
private:
	QString _fullScreenToImagePath;
public:
	FullScreenButton(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
};

