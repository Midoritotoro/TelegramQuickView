#pragma once

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

	void clearChannelsJsonArray();
	void saveUserData(QString& apiHash, QString& phoneNumber, QString& apiId);
	void saveTargetChannels(QStringList channels);
	void saveLastPostsCountForChannels(int count);
};