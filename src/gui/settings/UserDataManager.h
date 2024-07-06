﻿#pragma once


#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include "TelegramAuthorizationChecker.h"

class UserDataManager
{
private:
	QFile _jsonFile;
public:
	UserDataManager();

	[[nodiscard]] QJsonDocument getJsonDocument();
	[[nodiscard]] static const QString getUserSettingsPath();
	[[nodiscard]] QStringList getTelegramCredentials();
	[[nodiscard]] bool isTelegramCredentialsValid();
	[[nodiscard]] bool isTelegramPhoneNumberCodeValid();

	void clearChannelsJsonArray();
	void clearTelegramCredentials();
	void setTelegramCredentials(QString& apiHash, QString& phoneNumber, QString& apiId);
	void setTargetChannels(QStringList channels);
	void setLastPostsCountForChannels(int count);
	void setPhoneNumberCode(QString& code);
};