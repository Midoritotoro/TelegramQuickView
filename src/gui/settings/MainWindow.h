#pragma once

#include "UserDataManager.h"

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenuBar>

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>

#include "../../core/winapi/MouseDetector.h"

class MainWindow : public QWidget
{
private:
	Q_OBJECT
	QLineEdit* TelegramParserTargetLineEdit = nullptr;
	MouseDetector* mouseDetector = nullptr;
	UserDataManager* userDataManager = nullptr;
public:
	MainWindow(QWidget* parent = nullptr);
private Q_SLOTS:
	void on_AddChannelsButton_click();
	void on_ReplaceChannelsButton_click();
	
	void on_GetChannelsFromFileButton_click();

	void LeftPartOfScreenAction_triggered();
	void RightPartOfScreenAction_triggered();
};
