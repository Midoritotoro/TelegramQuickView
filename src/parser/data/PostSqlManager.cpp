#include "PostSqlManager.h"

#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>

PostSqlManager::PostSqlManager() {
	const auto dataBasePath = getDatabasePath();

	qDebug() << "Database path: " << dataBasePath;

	const auto sqlQuery = QString::fromUtf8(
		"CREATE TABLE IF NOT EXISTS " + std::string(DataBaseTableName) + "(\n"
			"id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
			"sender TEXT,\n"
			"attachments TEXT,\n"
			"date DATETIME,\n"
			"text TEXT,\n"
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
	if (_dataBase.open() == false) {
		qDebug() << "Ошибка открытия базы данных: " << _dataBase.lastError();
		return;
	}

	if (message.mediaAlbumId != 0)
		if (rowExists("mediaAlbumId", message.mediaAlbumId)) {
			qDebug() << "Row already exists: updating...";
			return updateMessageInfo(message);
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

	query.exec();

	_dataBase.commit();
	_dataBase.close();
}

void PostSqlManager::updateMessageInfo(const TelegramMessage& message) {
	if (_dataBase.open() == false) {
		qDebug() << "Ошибка открытия базы данных: " << _dataBase.lastError();
		return;
	}
	QSqlQuery query(_dataBase);

	const auto sqlUpdateQuery = QString::fromUtf8("UPDATE "
		+ std::string(DataBaseTableName)
		+ " SET attachments = COALESCE(attachments, '') || :attachments, text = COALESCE(text, '') || :text WHERE mediaAlbumId = :mediaAlbumId"
	);

	query.prepare(sqlUpdateQuery);

	qDebug() << query.lastError();

	query.bindValue(":attachments", ", " + message.attachment);
	query.bindValue(":text", message.text);
	query.bindValue(":mediaAlbumId", message.mediaAlbumId);

	query.exec();

	_dataBase.commit();
	_dataBase.close();
}

bool PostSqlManager::rowExists(const QString& columnName, const QVariant& parameter) {
	if (_dataBase.open() == false) {
		qDebug() << "Ошибка открытия базы данных: " << _dataBase.lastError();
		return false;
	}

	QSqlQuery query(_dataBase);

	const auto checkQuery = QString(
		"SELECT EXISTS(SELECT * FROM %1 WHERE %2 = :parameter LIMIT 1)"
	).arg(DataBaseTableName)
	 .arg(columnName);

	query.prepare(checkQuery);
	query.bindValue(":parameter", parameter);

	if (query.exec() && query.next()) {
		_dataBase.commit();
		return query.value(0).toBool();
	}

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
