#include "SqlReader.h"

#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>


SqlReader::SqlReader() {
	const auto dataBasePath = getDatabasePath();

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
	}
}

SqlReader::~SqlReader() {
	_dataBase.close();
}

TelegramMessage SqlReader::getMessage(int id) {
	TelegramMessage message;

	if (_dataBase.isOpen() == false) {
		qDebug() << "Ошибка открытия базы данных: " << _dataBase.lastError();
		return message;
	}

	if (!rowExists("id", id))
		return message;

	QSqlQuery query(_dataBase);

	const auto selectQuery = QString(
		"SELECT * FROM %1 WHERE id = :id LIMIT 1"
	).arg(DataBaseTableName);

	query.prepare(selectQuery);
	query.bindValue(":id", id);

	if (query.exec() && query.next()) {
		message.attachments = query.value("attachments").toString().split(", ");
		message.sender = query.value("sender").toString();
		message.date = query.value("date").toString();
		message.text = query.value("text").toString();
	}

	return message;
}

bool SqlReader::rowExists(const QString& columnName, const QVariant& parameter) {
	if (_dataBase.isOpen() == false) {
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


QString SqlReader::getDatabasePath() const {
#ifdef PROJECT_NAME
	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).replace(QCoreApplication::applicationName(), PROJECT_NAME);

	if (!appDataDir.mkpath("."))
		return "";

	return appDataDir.absolutePath() + "/downloadedPosts.sqlite3";
#else
	return "";
#endif
}
