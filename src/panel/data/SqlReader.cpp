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

		if (_dataBase.transaction())
			_dataBase.commit();
	}
}

SqlReader::~SqlReader() {
	_dataBase.close();
}

Telegram::Message SqlReader::getMessage(int id) {
	Telegram::Message message;

	if (_dataBase.isOpen() == false) {
		qDebug() << "Ошибка открытия базы данных: " << _dataBase.lastError();
		return message;
	}

	QSqlQuery query(_dataBase);

	const auto selectQuery = QString(
		"SELECT * FROM %1"
	).arg(DataBaseTableName);

	if (!query.exec(selectQuery)) {
		qDebug() << "Error while executing query: " << query.lastQuery() << " - " << _dataBase.lastError().text();
		return message;
	}

	while (query.next()) {
		if (query.value("id").toInt() == id) {
			if (query.value("attachments").toString().length() != 0)
				message.attachments = query.value("mediaAlbumId").toInt() != 0
						? query.value("attachments").toString().split(", ")
						: query.value("attachments").toStringList();

			message.sender = query.value("sender").toString();
			message.date = query.value("date").toString();
			message.text = query.value("text").toString();

			return message;
		}
	}

	return message;
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
