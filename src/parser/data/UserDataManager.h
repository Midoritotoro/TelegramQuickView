#pragma once

#include <QFile>
#include <QJsonDocument>


typedef struct _TelegramCredentials {
	int apiId = 0;
	std::string apiHash = "";
	std::string phoneNumber = "";

	[[nodiscard]] bool isEmpty() {
		return apiId == 0 && apiHash.empty() && phoneNumber.empty();
	}
} TelegramCredentials;


class UserDataManager final
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

	[[nodiscard]] QVariantList getUsernamesOfTargetChannels();
	[[nodiscard]] QVariantList getIdsOfTargetChannels();

	bool setTelegramCredentials(const TelegramCredentials& telegramCredentials);

	void setTargetChannels(QStringList usernameList);
	void setTargetChannelsChatIds(QList<qint64> idsList);

	void setLastPostsCountForChannels(int count);
private:
	void clearUsernamesOfChannels();
	void clearChatIdsOfChannels();

	void clearTelegramCredentials();

	void write(const QByteArray& data);
};
