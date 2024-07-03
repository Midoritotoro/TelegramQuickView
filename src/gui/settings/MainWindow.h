#pragma once

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
	QFile jsonFile;
	QString fileName;
	QDir writeDir;
public:
	MainWindow(QWidget* parent = nullptr);

	QJsonDocument getJsonDocument();
	
	void clearChannelsJsonArray();
	void saveUserData(QString& apiHash, QString& phoneNumber, QString& apiId);
	void saveTargetChannels(QStringList channels);
	void saveLastPostsCountForChannels(int count);
private Q_SLOTS:
	void on_AddChannelsButton_click();
	void on_ReplaceChannelsButton_click();
	
	void on_GetChannelsFromFileButton_click();

	void LeftPartOfScreenAction_triggered();
	void RightPartOfScreenAction_triggered();
};
