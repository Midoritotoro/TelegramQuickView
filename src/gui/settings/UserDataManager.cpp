﻿#include "UserDataManager.h"


UserDataManager::UserDataManager() {
	const QString fileName = getUserSettingsPath();
	_jsonFile.setFileName(fileName);
	_jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}

const QString UserDataManager::getUserSettingsPath()
{
	const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (!writeDir.mkpath("."))
		return "";
	const QString fileName = writeDir.absolutePath() + "\\userData.json";
	return fileName;
}

QJsonDocument UserDataManager::getJsonDocument() {
	QJsonDocument jsonDocument;
	QByteArray jsonData;

	
	if (!_jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return jsonDocument;

	jsonData = _jsonFile.readAll();
	_jsonFile.close();

	jsonDocument = QJsonDocument::fromJson(jsonData);
	return jsonDocument;
}

bool UserDataManager::isTelegramCredentialsValid() {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject = jsonDocument.object();

	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");

	TelegramAuthorizationChecker* telegramAuthorizationChecker = new TelegramAuthorizationChecker();
	bool isTelegramCredentialsValid = telegramAuthorizationChecker->callTelegramCredentialsValidCheck(apiHash.toString().toStdString().c_str(), phoneNumber.toString().toStdString().c_str(), apiId.toString().toInt());

	if (!apiHash.isUndefined() == true && !apiId.isUndefined() == true && !phoneNumber.isUndefined() == true && isTelegramCredentialsValid == true)
		return true;

	return false;
}
bool UserDataManager::isTelegramPhoneNumberCodeValid() {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject = jsonDocument.object();

	QJsonValue code = jsonObject.value("code");

	if (!code.isUndefined() == true)
		return true;

	return false;
}


void UserDataManager::clearChannelsJsonArray() {
	QJsonObject jsonObject;
	QJsonArray jsonArray;
	QJsonDocument jsonDocument = getJsonDocument();
	jsonObject = jsonDocument.object();

	if (jsonObject.value("channels").isNull())
		return;

	jsonObject.remove("channels");
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setTelegramCredentials(QString& apiHash, QString& phoneNumber, QString& apiId) {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject.insert("apiHash", apiHash);
	jsonObject.insert("phoneNumber", phoneNumber);
	jsonObject.insert("apiId", apiId);

	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setTargetChannels(QStringList channels) {
	QJsonArray jsonArray;
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject currentDocumentObject = jsonDocument.object();

	if (!currentDocumentObject.value("channels").toArray().isEmpty())
		jsonArray = currentDocumentObject.value("channels").toArray();

	clearChannelsJsonArray();

	foreach(const QString & channel, channels)
		jsonArray.append(channel);

	currentDocumentObject.insert("channels", jsonArray);
	jsonDocument.setObject(currentDocumentObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setLastPostsCountForChannels(int lastPostsCount) {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject = jsonDocument.object();

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setPhoneNumberCode(QString & code) {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	jsonObject = jsonDocument.object();

	jsonObject.insert("code", code);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}
