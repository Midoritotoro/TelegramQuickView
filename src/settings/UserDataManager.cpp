#include "UserDataManager.h"

#include <future>


UserDataManager::UserDataManager() {
	const auto fileName = getUserSettingsPath();

	_jsonFile.setFileName(fileName);
	_jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}

const QString UserDataManager::getUserSettingsPath()
{
	const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if (!writeDir.mkpath("."))
		return "";

	const auto fileName = writeDir.absolutePath() + "\\userData.json";
	return fileName;
}

QJsonDocument UserDataManager::getJsonDocument() {
	QJsonDocument jsonDocument;

	if (!_jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return jsonDocument;

	const auto jsonData = _jsonFile.readAll();
	_jsonFile.close();

	jsonDocument = QJsonDocument::fromJson(jsonData);
	return jsonDocument;
}

bool UserDataManager::isTelegramCredentialsValid() {
	const auto credentials = getTelegramCredentials();
	return QString::number(credentials.apiId).length() >= 5 == true
			&& credentials.apiHash.length() >= 32 == true;
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

std::string UserDataManager::getTelegramAuthCode() {
	const auto jsonDocument = getJsonDocument();
	const auto jsonObject = jsonDocument.object();

	const auto code = jsonObject.value("code");

	return code.toString().toStdString();
}

bool UserDataManager::isTelegramAuthCodeValid() {
	const auto code = getTelegramAuthCode();

	return code.length() >= 5 == true 
		&& isTelegramCredentialsValid() == true;
}


void UserDataManager::clearChannelsJsonArray() {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	if (jsonObject.value("channels").isNull())
		return;

	jsonObject.remove("channels");
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
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

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

bool UserDataManager::setTelegramCredentials(const TelegramCredentials& telegramCredentials) {
	if (telegramCredentials.apiHash.length() < 32 || QString::number(telegramCredentials.apiId).length() < 5)
		return false;

	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	jsonObject.insert("apiHash", telegramCredentials.apiHash.c_str());
	jsonObject.insert("phoneNumber", telegramCredentials.phoneNumber.c_str());
	jsonObject.insert("apiId", telegramCredentials.apiId);

	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();

	return true;
}

void UserDataManager::setTargetChannels(QStringList channels) {
	QJsonArray jsonArray;

	auto jsonDocument = getJsonDocument();
	auto currentDocumentObject = jsonDocument.object();

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
	auto jsonDocument = getJsonDocument();

	auto jsonObject = jsonDocument.object();

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setPhoneNumberCode(QString& code) {
	auto jsonDocument = getJsonDocument();
	auto jsonObject = jsonDocument.object();

	jsonObject.insert("code", code);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}