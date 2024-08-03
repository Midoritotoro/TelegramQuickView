#include "SqlContentManager.h"

#include <QSqlQuery>
#include <QSqlDataBase>
#include <QStandardPaths>
#include <QDir>

TelegramMessagesList SqlContentManager::getLastMessages() {
	TelegramMessagesList telegramMessagesList;

	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if (!appDataDir.mkpath("."))
		return ;

	const QString dataBasePath = appDataDir.absolutePath() + "\\downloadedPosts.sqlite3";

	QSqlDatabase dataBase = QSqlDatabase::addDatabase(dataBasePath);
	QSqlQuery query(dataBase);

	const QString sqlSelectQuery = "SELECT * FROM downloadedPosts";

	if (!query.exec(sqlSelectQuery))
		return;

	while (query.next()) {
		
		TelegramMessage telegramMessage;
		telegramMessage.text = query.value("text").toString();

		telegramMessagesList.append(telegramMessage);
	}
}
TelegramMessage SqlContentManager::getLastMessage() {
	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	if (!appDataDir.mkpath("."))
		return;

	const QString dataBasePath = appDataDir.absolutePath() + "\\downloadedPosts.sqlite3";

	QSqlDatabase dataBase = QSqlDatabase::addDatabase(dataBasePath);
	QSqlQuery query(dataBase);

	const QString sqlSelectQuery = "SELECT * FROM downloadedPosts";
}
