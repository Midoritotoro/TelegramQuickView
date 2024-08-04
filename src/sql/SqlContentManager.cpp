#include "SqlContentManager.h"

#include <QSqlQuery>
#include <QSqlDataBase>
#include <QStandardPaths>
#include <QDir>

TelegramMessagesList SqlContentManager::getLastMessages() {
	TelegramMessagesList telegramMessagesList;

	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if (!appDataDir.mkpath("."))
		return telegramMessagesList;

	const QString dataBasePath = appDataDir.absolutePath() + "\\downloadedPosts.sqlite3";

	QSqlDatabase dataBase = QSqlDatabase::addDatabase(dataBasePath);
	QSqlQuery query(dataBase);

	const QString sqlSelectQuery = "SELECT * FROM downloadedPosts";

	if (!query.exec(sqlSelectQuery))
		return telegramMessagesList;

	while (query.next()) {
		
		TelegramMessage telegramMessage;
		telegramMessage.text = query.value("text").toString();

		telegramMessagesList.append(telegramMessage);
	}
	return telegramMessagesList;
}
TelegramMessage SqlContentManager::getLastMessage() {
	TelegramMessage telegramMessage;

	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if (!appDataDir.mkpath("."))
		return telegramMessage;

	const QString dataBasePath = appDataDir.absolutePath() + "\\downloadedPosts.sqlite3";

	QSqlDatabase dataBase = QSqlDatabase::addDatabase(dataBasePath);
	QSqlQuery query(dataBase);

	const QString sqlSelectQuery = "SELECT * FROM downloadedPosts";

	return telegramMessage;
}
