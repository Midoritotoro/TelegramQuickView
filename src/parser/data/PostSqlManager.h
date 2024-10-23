#pragma once

#include <QSqlDatabase>

#include <QList>
#include <QUrl>
#include <QString>

#define DataBaseTableName "PostsInfo"

typedef struct _TelegramMessage {
	QString sender = "";
	QString text = "";
	QString date = "";
	QString attachment = "";
	int64_t mediaAlbumId = 0;
} TelegramMessage;


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