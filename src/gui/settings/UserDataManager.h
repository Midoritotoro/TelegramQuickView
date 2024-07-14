#pragma once

#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include "TelegramAuthorizationChecker.h"

typedef struct _TelegramCredentials {
	QString apiId;
	QString apiHash;
	QString phoneNumber;
} TelegramCredentials, * LPTelegramCredentials;

class UserDataManager
{
private:
	QFile _jsonFile;
public:
	UserDataManager();

	[[nodiscard]] QJsonDocument getJsonDocument();
	[[nodiscard]] static const QString getUserSettingsPath();
	[[nodiscard]] LPTelegramCredentials getTelegramCredentials();
	[[nodiscard]] bool isTelegramCredentialsValid();
	[[nodiscard]] bool isTelegramPhoneNumberCodeValid();

	void clearChannelsJsonArray();
	void clearTelegramCredentials();

	[[nodiscard]] bool setTelegramCredentials(LPTelegramCredentials telegramCredentials);
	void setTargetChannels(QStringList channels);
	void setLastPostsCountForChannels(int count);
	void setPhoneNumberCode(QString& code);
};