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

	qDebug() << apiHash;
	qDebug() << apiId;
	qDebug() << phoneNumber;

	TelegramAuthorizationChecker* telegramAuthorizationChecker = new TelegramAuthorizationChecker();
	bool isTelegramCredentialsValid = telegramAuthorizationChecker->TelegramCredentialsValidCheck(apiHash.toString().toStdString().c_str(), phoneNumber.toString().toStdString().c_str(), apiId.toString().toInt());

	return !apiHash.isUndefined() == true && !apiId.isUndefined() == true && !phoneNumber.isUndefined() == true && isTelegramCredentialsValid == true;
}
bool UserDataManager::isTelegramPhoneNumberCodeValid() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	QJsonValue code = jsonObject.value("code");

	if (!code.isUndefined() == true)
		return true;
	return false;
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
	QJsonDocument jsonDocument = getJsonDocument();

	QJsonObject jsonObject = jsonDocument.object();

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}

void UserDataManager::setPhoneNumberCode(QString & code) {
	QJsonDocument jsonDocument = getJsonDocument();

	QJsonObject jsonObject = jsonDocument.object();

	jsonObject.insert("code", code);
	jsonDocument.setObject(jsonObject);

	_jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	_jsonFile.write(jsonDocument.toJson());
	_jsonFile.close();
}
