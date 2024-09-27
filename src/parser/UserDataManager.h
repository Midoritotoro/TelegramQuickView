#pragma once

#include <QFile>
#include <QJsonDocument>

#include "TelegramAuthorizer.h"


class UserDataManager
{
private:
	QFile _jsonFile;
	TelegramCredentials _telegramCredentials;
public:
	UserDataManager();

	[[nodiscard]] QJsonDocument getJsonDocument();

	[[nodiscard]] static QString getUserSettingsPath();
	[[nodiscard]] TelegramCredentials getTelegramCredentials();
	[[nodiscard]] std::string getTelegramAuthCode();

	[[nodiscard]] bool isTelegramCredentialsValid();
	[[nodiscard]] bool isTelegramAuthCodeValid();

	void clearChannelsJsonArray();
	void clearTelegramCredentials();

	bool setTelegramCredentials(const TelegramCredentials& telegramCredentials);

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