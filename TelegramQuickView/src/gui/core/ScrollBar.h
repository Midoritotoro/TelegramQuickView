#pragma once

#include <QtWidgets>
#include <QtCore>

#include <windows.h>

#include "../settings/DynamicIconChannelButton.h"
#include "../settings/DynamicScrollArea.h"

class ScrollBar: public QWidget
{
private:
	Q_OBJECT
	DynamicScrollArea* ScrollArea = nullptr, * IconScrollArea = nullptr;
	QGridLayout* GridLayout = nullptr, * mainLayout = nullptr;
	QVBoxLayout* vLayout = nullptr;
	QWidget* mainWidget = nullptr, * vMainWidget = nullptr;
	QHBoxLayout* hBoxLayout = nullptr;
public:
	ScrollBar(QWidget* parent = nullptr);
	~ScrollBar();
	void CreateScrollBar();
	void CreateTelegramChannelWidget(const QString channelName, const QImage& image, QWidget* parent = nullptr);
private slots:
	void DynamicIconButton_clicked();
};
