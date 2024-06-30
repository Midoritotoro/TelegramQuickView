#pragma once

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenuBar>

#include "../../core/winapi/MouseDetector.h"


class MainWindow : public QWidget
{
private:
	Q_OBJECT
	QDialog* DialogWindow = nullptr;
	QGridLayout* GridLayout = nullptr;
	QLabel* TelegramParserTextLabel = nullptr;
	QLineEdit* TelegramParserTargetLineEdit = nullptr;
	QTextEdit* ReadedChannelsTextEdit = nullptr;
	QPushButton* GetChannelsFromFileButton = nullptr, * AddChannelsButton = nullptr, * ReplaceChannelsButton = nullptr;
	QMenuBar* channelOptionsMenuBar = nullptr;
	QMenu* channelOptionsMenu = nullptr;
	QAction* LeftPartOfScreenAction = nullptr, * RightPartOfScreenAction = nullptr;
	MouseDetector* mouseDetector = nullptr;
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	void CreateMainWindow();
private slots:
	void on_AddChannelsButton_click();
	void on_ReplaceChannelsButton_click();
	void on_GetChannelsFromFileButton_click();

	void LeftPartOfScreenAction_triggered();
	void RightPartOfScreenAction_triggered();
};