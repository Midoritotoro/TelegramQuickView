#include "PostSqlManager.h"

#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QSqlQuery>


PostSqlManager::PostSqlManager() {
	const auto dataBasePath = getDatabasePath();
	const auto sqlQuery = QString::fromUtf8(
		"CREATE TABLE IF NOT EXISTS " + std::string(DataBaseTableName) + "(\n"
			"id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
			"sender TEXT,\n"
			"attachments TEXT,\n"
			"date DATETIME,\n"
			"text TEXT\n"
			"mediaAlbumId INTEGER\n"
		");"
	);

	_dataBase = QSqlDatabase::addDatabase("QSQLITE");
	_dataBase.setDatabaseName(dataBasePath);

	if (_dataBase.open()) {
		QSqlQuery query(_dataBase);
		query.exec(sqlQuery);

		_dataBase.commit();
		_dataBase.close();
	}
}

void PostSqlManager::writeMessageInfo(const TelegramMessage& message) {
	if (_dataBase.open() == false)
		return;

	if (message.mediaAlbumId != 0)
		if (rowExists(":mediaAlbumId", message.mediaAlbumId)) {
			updateMessageInfo(message);
			return;
		}

	QSqlQuery query(_dataBase);

	const auto sqlInsertQuery = QString::fromUtf8("INSERT INTO "
		+ std::string(DataBaseTableName)
		+ "(sender, attachments, date, text, mediaAlbumId) VALUES\n"
		+ "(:sender, :attachments, :date, :text, :mediaAlbumId)"
	);

	query.prepare(sqlInsertQuery);

	query.bindValue(":sender", message.sender);
	query.bindValue(":attachments", message.attachment);
	query.bindValue(":date", message.date);
	query.bindValue(":text", message.text);
	query.bindValue(":mediaAlbumId", message.mediaAlbumId);

	_dataBase.commit();
	_dataBase.close();
}

void PostSqlManager::updateMessageInfo(const TelegramMessage& message) {
	if (_dataBase.open() == false)
		return;

	QSqlQuery query(_dataBase);

	const auto sqlUpdateQuery = QString::fromUtf8("UPDATE "
		+ std::string(DataBaseTableName)
		+ "SET attachments = :attachments AND text = :text WHERE mediaAlbumId = :mediaAlbumId"
	);

	query.prepare(sqlUpdateQuery);

	query.bindValue(":attachments", ", " + message.attachment);
	query.bindValue(":text", message.text);
	query.bindValue(":mediaAlbumId", message.mediaAlbumId);

	_dataBase.commit();
	_dataBase.close();
}

bool PostSqlManager::rowExists(const QString& columnName, const QVariant& parameter) {
	QSqlQuery query(_dataBase);

	const auto checkQuery = QString::fromUtf8("SELECT COUNT(*) FROM"
		+ std::string(DataBaseTableName)
		+ "WHERE %2 = :parameter"
	);

	query.prepare(checkQuery);
	query.bindValue(":parameter", parameter);

	if (query.exec() && query.next())
		return query.value(0).toInt() > 0;

	return false;
}


QString PostSqlManager::getDatabasePath() {
#ifdef PROJECT_NAME
	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).replace(QCoreApplication::applicationName(), PROJECT_NAME);

	if (!appDataDir.mkpath("."))
		return "";

	return appDataDir.absolutePath() + "/downloadedPosts.sqlite3";
#else
	return "";
#endif
}


