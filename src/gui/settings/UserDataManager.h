#pragma once

#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>

#include "TelegramAuthorizationChecker.h"


class UserDataManager
{
public:
	[[nodiscard]] static QJsonDocument getJsonDocument();
	[[nodiscard]] static const QString getUserSettingsPath();
	[[nodiscard]] static LPTelegramCredentials getTelegramCredentials();
	[[nodiscard]] static DWORD WINAPI isTelegramCredentialsValid(LPVOID lpParam = nullptr);
	[[nodiscard]] static DWORD WINAPI isTelegramPhoneNumberCodeValid(LPVOID lpParam = nullptr);

	static void clearChannelsJsonArray();
	static void clearTelegramCredentials();

	[[nodiscard]] static bool setTelegramCredentials(LPTelegramCredentials telegramCredentials);
	static void setTargetChannels(QStringList channels);
	static void setLastPostsCountForChannels(int count);
	static void setPhoneNumberCode(QString& code);
};