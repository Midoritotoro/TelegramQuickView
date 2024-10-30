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
	QStringList attachments = {};

	[[nodiscard]] bool isNull() const {
		return text.isEmpty() && attachments.isEmpty()
				&& sender.isEmpty() && date.isEmpty();
	}
} TelegramMessage;


class SqlReader {
private:
	QSqlDatabase _dataBase;
public:
	SqlReader();
	~SqlReader();

	[[nodiscard]] TelegramMessage getMessage(int id);
private:
	[[nodiscard]] bool rowExists(const QString& columnName, const QVariant& parameter);

	[[nodiscard]] QString getDatabasePath() const;
};