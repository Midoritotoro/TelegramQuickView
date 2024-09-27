#pragma once

#include <QWidget>
#include <QLineEdit>

//#include "../parser/UserDataManager.h"

class MainWindow : public QWidget
{
private:
	Q_OBJECT
	QLineEdit* TelegramParserTargetLineEdit = nullptr;
	//UserDataManager* userDataManager = nullptr;
public:
	MainWindow(QWidget* parent = nullptr);
public Q_SLOTS:
	void on_AddChannelsButton_click();
	void on_ReplaceChannelsButton_click();

	void on_GetChannelsFromFileButton_click();
};