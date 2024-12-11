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
public:
	UserDataManager();

	[[nodiscard]] std::string getTelegramAuthCode();

	bool setTelegramCredentials(
		const TelegramCredentials& telegramCredentials);
	[[nodiscard]] TelegramCredentials getTelegramCredentials();

	[[nodiscard]] bool isTelegramCredentialsValid();

	void setDownloadSensitiveContentAbility(bool ability);
	[[nodiscard]] bool getDownloadSensitiveContentAbility();

	void setTargetChannels(QStringList usernameList);
	[[nodiscard]] QVariantList getUsernamesOfTargetChannels();

	void setTargetChannelsChatIds(QList<qint64> idsList);
	[[nodiscard]] QVariantList getIdsOfTargetChannels();

	void setLastPostsCountForChannels(int count);
	[[nodiscard]] int getCountOfLatestDownloadingMessages();
private:
	[[nodiscard]] QJsonDocument getJsonDocument();
	[[nodiscard]] static QString getUserSettingsPath() const noexcept;

	void clearUsernamesOfChannels();
	void clearChatIdsOfChannels();
	void clearTelegramCredentials();

	void write(const QByteArray& data);

	QFile _jsonFile;
	TelegramCredentials _telegramCredentials;
};
