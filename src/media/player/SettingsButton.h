#pragma once

#include <QWidget>

class SettingsButton: public QWidget {
	Q_OBJECT
public:
	SettingsButton(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
};