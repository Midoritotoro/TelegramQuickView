﻿#pragma once

#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include "TelegramAuthorizationChecker.h"
#include "TelegramAuthorizer.h"


class UserDataManager
{
private:
	QFile _jsonFile;
	TelegramCredentials _telegramCredentials;
public:
	UserDataManager();

	[[nodiscard]] QJsonDocument getJsonDocument();
	[[nodiscard]] static const QString getUserSettingsPath();
	[[nodiscard]] TelegramCredentials getTelegramCredentials();
	[[nodiscard]] bool isTelegramCredentialsValid();
	[[nodiscard]] bool isTelegramPhoneNumberCodeValid();

	void clearChannelsJsonArray();
	void clearTelegramCredentials();

	[[nodiscard]] bool setTelegramCredentials(LPTelegramCredentials telegramCredentials);
	void setTargetChannels(QStringList channels);
	void setLastPostsCountForChannels(int count);
	void setPhoneNumberCode(QString& code);
};

//TelegramAuthorizer telegramAuthorizer;
//TelegramCredentials telegramCredentials;
//
//telegramCredentials.apiId = apiId.toInt();
//telegramCredentials.apiHash = apiHash.toString().toStdString();
//telegramCredentials.phoneNumber = phoneNumber.toString().toStdString();
//
//bool isTelegramCredentialsValid = telegramAuthorizer.setTelegramCredentials(telegramCredentials);