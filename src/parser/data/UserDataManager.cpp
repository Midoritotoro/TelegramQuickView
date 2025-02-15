﻿#include "UserDataManager.h"

#include <QDir>
#include <QStandardPaths>

#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>


UserDataManager::UserDataManager() {
	const auto fileName = getUserSettingsPath();

	_jsonFile.setFileName(fileName);
	_jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}

bool UserDataManager::setTelegramCredentials(const TelegramCredentials& telegramCredentials) {
	if (telegramCredentials.apiHash.length() < 32
		|| QString::number(telegramCredentials.apiId).length() < 5
		|| telegramCredentials.phoneNumber.length() < 10)
		return false;

	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	jsonObject.insert("apiHash", telegramCredentials.apiHash.c_str());
	jsonObject.insert("phoneNumber", telegramCredentials.phoneNumber.c_str());
	jsonObject.insert("apiId", telegramCredentials.apiId);

	jsonDocument.setObject(jsonObject);

	write(jsonDocument.toJson());

	return true;
}

TelegramCredentials UserDataManager::getTelegramCredentials() {
	if (_telegramCredentials.isEmpty()) {
		const auto jsonDocument = getJsonDocument();
		const auto jsonObject = jsonDocument.object();

		const auto apiHash = jsonObject.value("apiHash");
		const auto apiId = jsonObject.value("apiId");
		const auto phoneNumber = jsonObject.value("phoneNumber");

		_telegramCredentials.apiHash = apiHash.toString().toStdString();
		_telegramCredentials.apiId = apiId.toInt();
		_telegramCredentials.phoneNumber = phoneNumber.toString().toStdString();
	}

	return _telegramCredentials;
}

bool UserDataManager::isTelegramCredentialsValid() {
	const auto credentials = getTelegramCredentials();
	return QString::number(credentials.apiId).length() >= 5
		&& credentials.apiHash.length() >= 32
		&& credentials.phoneNumber.length() >= 10;
}

std::string UserDataManager::getTelegramAuthCode() {
	const auto jsonDocument = getJsonDocument();
	const auto jsonObject = jsonDocument.object();

	const auto code = jsonObject.value("code");

	return code.toString().toStdString();
}

void UserDataManager::setDownloadSensitiveContentAbility(bool ability) {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	jsonObject.insert("downloadSensitiveContent", ability);
	jsonDocument.setObject(jsonObject);

	write(jsonDocument.toJson());
}

bool UserDataManager::getDownloadSensitiveContentAbility() {
	const auto jsonDocument = getJsonDocument();
	const auto jsonObject = jsonDocument.object();

	return jsonObject.value("downloadSensitiveContent").toBool();
}

void UserDataManager::setTargetChannels(QStringList usernameList) {
	QJsonArray jsonArray;
	QJsonObject channelsObject;

	auto jsonDocument = getJsonDocument();
	auto currentDocumentObject = jsonDocument.object();

	if (currentDocumentObject.contains("channels")) 
		channelsObject = currentDocumentObject.value("channels").toObject();
	else 
		channelsObject = QJsonObject();

	if (channelsObject.contains("usernames"))
		jsonArray = channelsObject.value("usernames").toArray();

	clearUsernamesOfChannels();

	foreach(const auto& channel, usernameList)
		if (jsonArray.contains(channel) == false)
			jsonArray.append(channel);

	channelsObject.insert("usernames", jsonArray);
	currentDocumentObject.insert("channels", channelsObject);

	jsonDocument.setObject(currentDocumentObject);

	write(jsonDocument.toJson());
}

QVariantList UserDataManager::getUsernamesOfTargetChannels() {
	const auto jsonDocument = getJsonDocument();
	const auto jsonObject = jsonDocument.object();

	return jsonObject.value("channels").
		toObject().value("usernames").
		toArray().toVariantList();
}

void UserDataManager::setTargetChannelsChatIds(QList<qint64> idsList) {
	QJsonArray jsonArray;
	QJsonObject channelsObject;

	auto jsonDocument = getJsonDocument();
	auto currentDocumentObject = jsonDocument.object();

	if (currentDocumentObject.contains("channels"))
		channelsObject = currentDocumentObject.value("channels").toObject();
	else
		channelsObject = QJsonObject();


	if (channelsObject.contains("ids"))
		jsonArray = channelsObject.value("ids").toArray();

	clearChatIdsOfChannels();

	foreach(const auto& id, idsList)
		if (jsonArray.contains(id) == false)
			jsonArray.append(id);

	channelsObject.insert("ids", jsonArray);
	currentDocumentObject.insert("channels", channelsObject);

	jsonDocument.setObject(currentDocumentObject);

	write(jsonDocument.toJson());
}

QVariantList UserDataManager::getIdsOfTargetChannels() {
	const auto jsonDocument = getJsonDocument();
	const auto jsonObject = jsonDocument.object();

	return jsonObject.value("channels").
		toObject().value("ids").
		toArray().toVariantList();
}

void UserDataManager::setLastPostsCountForChannels(int lastPostsCount) {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	write(jsonDocument.toJson());
}

int UserDataManager::getCountOfLatestDownloadingMessages() {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	const auto count = jsonObject.value("lastPostsCount");

	return !count.isNull()
		? count.toInt()
		: 1;
}


QString UserDataManager::getUserSettingsPath()
{
#ifdef PROJECT_NAME
	const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
		.replace(QCoreApplication::applicationName(), PROJECT_NAME);

	if (!writeDir.mkpath("."))
		return "";

	return writeDir.absolutePath() + "/userData.json";
#else
	return "";
#endif
}

QJsonDocument UserDataManager::getJsonDocument() {
	QJsonDocument jsonDocument;

	if (!_jsonFile.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::ReadWrite))
		return jsonDocument;

	const auto jsonData = _jsonFile.readAll();

	jsonDocument = QJsonDocument::fromJson(jsonData);
	_jsonFile.close();

	return jsonDocument;
}

void UserDataManager::clearUsernamesOfChannels() {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	auto channelsObject = jsonObject.value("channels").toObject();

	if (channelsObject.isEmpty())
		return;

	if (channelsObject.value("usernames").isNull() == false)
		channelsObject.remove("usernames");

	jsonDocument.setObject(jsonObject);

	write(jsonDocument.toJson());
}

void UserDataManager::clearChatIdsOfChannels() {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	auto channelsObject = jsonObject.value("channels").toObject();

	if (channelsObject.isEmpty())
		return;

	if (channelsObject.value("ids").isNull() == false)
		channelsObject.remove("ids");

	jsonDocument.setObject(jsonObject);

	write(jsonDocument.toJson());
}

void UserDataManager::clearTelegramCredentials() {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	if (jsonObject.value("apiId").isNull() == false)
		jsonObject.remove("apiId");

	if (jsonObject.value("apiHash").isNull() == false)
		jsonObject.remove("apiHash");

	if (jsonObject.value("phoneNumber").isNull() == false)
		jsonObject.remove("phoneNumber");

	jsonDocument.setObject(jsonObject);

	write(jsonDocument.toJson());
}


void UserDataManager::write(const QByteArray& data) {
	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);

	if (_jsonFile.isOpen() == false)
		return;

	_jsonFile.write(data);
	_jsonFile.close();
}