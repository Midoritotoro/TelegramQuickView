#pragma once

#include <QtWidgets>
#include <QtCore>

#include <windows.h>

#include "../settings/IconChannelLabel.h"

class ScrollBar: public QWidget
{
private:
	Q_OBJECT
	int ScreenHeight, ScreenWidth;
	QScrollArea* ScrollArea = nullptr, * IconScrollArea = nullptr;
	QGridLayout* GridLayout = nullptr, * mainLayout = nullptr;
	QVBoxLayout* vLayout = nullptr;
	QWidget* mainWidget = nullptr, * vMainWidget = nullptr;
	QHBoxLayout* hBoxLayout = nullptr;;
public:
	ScrollBar(QWidget* parent = nullptr);
	~ScrollBar();
	void CreateScrollBar();
	void CreateTelegramChannelWidget(const QString channelName, const QString imagePath, QWidget* parent = nullptr);
private slots:
	void IconChannelLabel_clicked();
};
