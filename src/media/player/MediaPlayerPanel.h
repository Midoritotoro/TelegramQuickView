#pragma once

#include <QWidget>


class MediaPlayerPanel: public QWidget {
Q_OBJECT
private:
public:
	MediaPlayerPanel(QWidget* parent = nullptr);
protected:
	void updateSize();
	void paintEvent(QPaintEvent* event) override;
};