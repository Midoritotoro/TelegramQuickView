#pragma once

#include <QSqlDatabase>

#include <QList>
#include <QUrl>
#include <QString>

#define DataBaseTableName "PostsInfo"

typedef struct _TelegramMessage {
	QString sender;
	QString text;
	QString date;
	QStringList attachments;
} TelegramMessage;


class PostSqlManager {
private:
public:
	PostSqlManager();
	
	void saveMessageInfo(const TelegramMessage& message);
private:
	[[nodiscard]] QString getDatabasePath();
};