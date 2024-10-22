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
		");"
	);

	QSqlDatabase dataBase = QSqlDatabase::addDatabase("QSQLITE");
	dataBase.setDatabaseName(dataBasePath);

	if (dataBase.open()) {
		QSqlQuery query(dataBase);
		query.exec(sqlQuery);

		dataBase.commit();
		dataBase.close();
	}
}

void PostSqlManager::saveMessageInfo(const TelegramMessage& message) {
	const auto dataBasePath = getDatabasePath();

	QSqlDatabase dataBase = QSqlDatabase::addDatabase("QSQLITE");
	dataBase.setDatabaseName(dataBasePath);

	if (dataBase.open() == false)
		return;

	QSqlQuery query(dataBase);

	const auto sqlInsertQuery = QString::fromUtf8("INSERT INTO "
		+ std::string(DataBaseTableName)
		+ "(sender, attachments, date, text) VALUES\n"
		+ "(:sender, :attachments, :date, :text)"
	);

	query.prepare(sqlInsertQuery);

	QString attachments;
	const auto attachmentsLength = message.attachments.length();

	foreach (auto attachment, message.attachments)
		attachments.append(message.attachments.indexOf(attachment) < attachmentsLength ? attachment + ", " : attachment);

	query.bindValue(":sender", message.sender);
	query.bindValue(":attachments", attachments);
	query.bindValue(":date", message.date);
	query.bindValue(":text", message.text);

	dataBase.commit();
	dataBase.close();
}

QString PostSqlManager::getDatabasePath() {
#ifdef PROJECT_NAME
	const QDir appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).replace(QCoreApplication::applicationName(), PROJECT_NAME);

	if (!appDataDir.mkpath("."))
		return "";

	return appDataDir.absolutePath() + "/downloadedPosts.sqlite3";
#endif
	return "";
}


