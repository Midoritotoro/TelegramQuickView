#include "UserDataManager.h"


const QString UserDataManager::getUserSettingsPath()
{
	const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (!writeDir.mkpath("."))
		return "";
	const QString fileName = writeDir.absolutePath() + "/userData.json";
	return fileName;
}

QJsonDocument UserDataManager::getJsonDocument() {
	QJsonDocument jsonDocument;
	QFile jsonFile;
	
	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return jsonDocument;

	QByteArray jsonData = jsonFile.readAll();

	jsonDocument = QJsonDocument::fromJson(jsonData);
	jsonFile.close();
	return jsonDocument;
}

DWORD UserDataManager::isTelegramCredentialsValid(LPVOID lpParam) {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();

	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");

	if (apiHash.isUndefined() == true || apiId.isUndefined() == true || phoneNumber.isUndefined() == true)
		return 0;

	if (apiHash.toString().length() < 32 || QString::number(apiId.toInt()).length() < 5)
		return 0;

	LPTelegramCredentials telegramCredentials = new TelegramCredentials();
	telegramCredentials->apiHash = apiHash.toString().toStdString();
	telegramCredentials->phoneNumber = phoneNumber.toString().toStdString();
	telegramCredentials->apiId = apiId.toInt();

	DWORD isTelegramCredentialsValid = TelegramAuthorizationChecker::TelegramCredentialsValidCheck(telegramCredentials);
	qDebug() << isTelegramCredentialsValid;

	if (!apiHash.isUndefined() == true && !apiId.isUndefined() == true &&
		!phoneNumber.isUndefined() == true && isTelegramCredentialsValid == 1)
		return 1;
}

LPTelegramCredentials UserDataManager::getTelegramCredentials() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	LPTelegramCredentials telegramCredentials = new TelegramCredentials();
	
	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");

	telegramCredentials->apiHash = apiHash.toString().toStdString();
	telegramCredentials->apiId = apiId.toInt();
	telegramCredentials->phoneNumber = phoneNumber.toString().toStdString();

	return telegramCredentials;
}

DWORD UserDataManager::isTelegramPhoneNumberCodeValid(LPVOID lpParam) {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	LPTelegramCredentials telegramCredentials = new TelegramCredentials();

	QJsonValue apiHash = jsonObject.value("apiHash");
	QJsonValue apiId = jsonObject.value("apiId");
	QJsonValue phoneNumber = jsonObject.value("phoneNumber");
	QJsonValue code = jsonObject.value("code");
	QJsonValue codeHash = jsonObject.value("codeHash");

	telegramCredentials->apiHash = apiHash.toString().toStdString();
	telegramCredentials->phoneNumber = phoneNumber.toString().toStdString();
	telegramCredentials->apiId = apiId.toInt();
	telegramCredentials->code = code.toInt();
	telegramCredentials->codeHash = codeHash.toString().toStdString();

	if (apiHash.isUndefined() == true || apiId.isUndefined() == true || phoneNumber.isUndefined() == true)
		return false;

	if (apiHash.toString().length() < 32 || QString::number(apiId.toInt()).length() < 5)
		return false;

	bool isTelegramCodeValid = TelegramAuthorizationChecker::TelegramCodeValidCheck(telegramCredentials);

	if (code.toString().length() < 5)
		return false;

	return !code.isUndefined() == true && !codeHash.isUndefined() == true && isTelegramCodeValid == true;
}


void UserDataManager::clearChannelsJsonArray() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	QFile jsonFile;

	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	if (jsonObject.value("channels").isNull())
		return;

	jsonObject.remove("channels");
	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void UserDataManager::clearTelegramCredentials() {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	QFile jsonFile;

	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	if (!jsonObject.value("apiId").isNull())
		jsonObject.remove("apiId");

	if (!jsonObject.value("apiHash").isNull())
		jsonObject.remove("apiHash");

	if (!jsonObject.value("phoneNumber").isNull())
		jsonObject.remove("phoneNumber");

	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

bool UserDataManager::setTelegramCredentials(LPTelegramCredentials telegramCredentials) {
	QJsonDocument jsonDocument = getJsonDocument();
	QFile jsonFile;

	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	if (QString(telegramCredentials->apiHash.c_str()).length() < 32 || QString::number(telegramCredentials->apiId).length() < 5)
		return false;

	QJsonObject jsonObject = jsonDocument.object();

	jsonObject.insert("apiHash", QString(telegramCredentials->apiHash.c_str()));
	jsonObject.insert("phoneNumber", QString(telegramCredentials->phoneNumber.c_str()));
	jsonObject.insert("apiId", telegramCredentials->apiId);

	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();

	return true;
}

void UserDataManager::setTargetChannels(QStringList channels) {
	QJsonArray jsonArray;
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject currentDocumentObject = jsonDocument.object();
	QFile jsonFile;

	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	if (!currentDocumentObject.value("channels").toArray().isEmpty())
		jsonArray = currentDocumentObject.value("channels").toArray();

	clearChannelsJsonArray();

	foreach(const QString & channel, channels)
		jsonArray.append(channel);

	currentDocumentObject.insert("channels", jsonArray);
	jsonDocument.setObject(currentDocumentObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void UserDataManager::setLastPostsCountForChannels(int lastPostsCount) {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	QFile jsonFile;

	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	jsonObject.insert("lastPostsCount", lastPostsCount);
	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}

void UserDataManager::setPhoneNumberCode(QString& code) {
	QJsonDocument jsonDocument = getJsonDocument();
	QJsonObject jsonObject = jsonDocument.object();
	QFile jsonFile;

	const QString fileName = getUserSettingsPath();
	jsonFile.setFileName(fileName);
	jsonFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);

	jsonObject.insert("code", code);
	jsonDocument.setObject(jsonObject);

	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);
	jsonFile.write(jsonDocument.toJson());
	jsonFile.close();
}
