﻿#pragma once

// #include "../../src/core/parser/PythonCaller.h"

#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>


class UserDataManager
{
private:
	QFile _jsonFile;
public:
	UserDataManager();

	[[nodiscard]] QJsonDocument getJsonDocument();
	[[nodiscard]] static const QString getUserSettingsPath();
	[[nodiscard]] bool isUserAuthorized();

	void clearChannelsJsonArray();
	void authorize(QString& apiHash, QString& phoneNumber, QString& apiId);
	void saveTargetChannels(QStringList channels);
	void saveLastPostsCountForChannels(int count);
};