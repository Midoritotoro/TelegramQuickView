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
	~PostSqlManager();
	
	void writeMessageInfo(const Telegram::Message& message);
private:
	void updateMessageInfo(const Telegram::Message& message);
	[[nodiscard]] bool rowExists(const QString& columnName, const QVariant& parameter);

	[[nodiscard]] QString getDatabasePath() const;
};