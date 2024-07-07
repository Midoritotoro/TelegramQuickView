#include "UserDataManager.h"


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

	if (!_jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return jsonDocument;

	QByteArray jsonData = _jsonFile.readAll();
	_jsonFile.close();

	jsonDocument = QJsonDocument::fromJson(jsonData);
	return jsonDocument;
}

bool UserDataManager::isTelegramCredentialsValid() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");

	if (apiHash.isUndefined() == true || apiId.isUndefined() == true || phoneNumber.isUndefined() == true)
		return false;

	if (strlen(apiHash.toString().toStdString().c_str()) < 32 || strlen(apiId.toString().toStdString().c_str()) < 5)
		return false;

	TelegramAuthorizationChecker* telegramAuthorizationChecker = new TelegramAuthorizationChecker();
	bool isTelegramCredentialsValid = telegramAuthorizationChecker->TelegramCredentialsValidCheck(apiHash.toString().toStdString().c_str(), phoneNumber.toString().toStdString().c_str(), apiId.toString().toInt());

	return !apiHash.isUndefined() == true && !apiId.isUndefined() == true &&
		!phoneNumber.isUndefined() == true && isTelegramCredentialsValid == true;
}
LPTelegramCredentials UserDataManager::getTelegramCredentials() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	LPTelegramCredentials telegramCredentials = new TelegramCredentials();
	

	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");

	telegramCredentials->apiHash = apiHash.toString();
	telegramCredentials->apiId = apiId.toString();
	telegramCredentials->phoneNumber = phoneNumber.toString();

	return telegramCredentials;
}

bool UserDataManager::isTelegramPhoneNumberCodeValid() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");
	QJsonValue code = jsonObject.value("code");
	QJsonValue codeHash = jsonObject.value("codeHash");

	if (apiHash.isUndefined() == true || apiId.isUndefined() == true || phoneNumber.isUndefined() == true)
		return false;

	if (strlen(apiHash.toString().toStdString().c_str()) < 32 || strlen(apiId.toString().toStdString().c_str()) < 5)
		return false;

	TelegramAuthorizationChecker* telegramAuthorizationChecker = new TelegramAuthorizationChecker();
	bool isTelegramCodeValid = telegramAuthorizationChecker->TelegramCodeValidCheck(apiHash.toString().toStdString().c_str(), phoneNumber.toString().toStdString().c_str(), apiId.toString().toInt(), code.toString().toInt(), codeHash.toString().toStdString().c_str());

	if (strlen(code.toString().toStdString().c_str()) < 5)
		return false;

	return !code.isUndefined() == true && !codeHash.isUndefined() == true && isTelegramCodeValid == true;
}


void UserDataManager::clearChannelsJsonArray() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	if (jsonObject.value("channels").isNull())
		return;

	jsonObject.remove("channels");
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::clearTelegramCredentials() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	if (!jsonObject.value("apiId").isNull())
		jsonObject.remove("apiId");

	if (!jsonObject.value("apiHash").isNull())
		jsonObject.remove("apiHash");

	if (!jsonObject.value("phoneNumber").isNull())
		jsonObject.remove("phoneNumber");

	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

bool UserDataManager::setTelegramCredentials(_TelegramCredentials* telegramCredentials) {
	QJsonObject jsonObject;
	QJsonDocument jsonDocument = getJsonDocument();

	if (strlen(telegramCredentials->apiHash.toStdString().c_str()) < 32 || strlen(telegramCredentials->apiId.toStdString().c_str()) < 5)
		return false;

	jsonObject.insert("apiHash", telegramCredentials->apiHash);
	jsonObject.insert("phoneNumber", telegramCredentials->phoneNumber);
	jsonObject.insert("apiId", telegramCredentials->apiId);

	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
	return true;
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
	QJsonDocument jsonDocument = getJsonDocument();

	QJsonObject jsonObject = jsonDocument.object();

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setPhoneNumberCode(QString& code) {
	QJsonDocument jsonDocument = getJsonDocument();

	QJsonObject jsonObject = jsonDocument.object();

	jsonObject.insert("code", code);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}
