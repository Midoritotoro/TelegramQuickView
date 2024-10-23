#pragma once

#include <QSqlDatabase>

#include <QList>
#include <QUrl>
#include <QString>

#include "../utils.h"
#define DataBaseTableName "PostsInfo"


class PostSqlManager {
private:
	QSqlDatabase _dataBase;
public:
	PostSqlManager();
	
	void writeMessageInfo(const TelegramMessage& message);
private:
	void updateMessageInfo(const TelegramMessage& message);

	[[nodiscard]] bool rowExists(const QString& columnName, const QVariant& parameter);

	[[nodiscard]] QString getDatabasePath();
};