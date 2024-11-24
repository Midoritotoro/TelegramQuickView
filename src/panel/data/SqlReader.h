#pragma once

#include <QSqlDatabase>

#include <QList>
#include <QUrl>
#include <QString>

#include "../../parser/utils.h"

#define DataBaseTableName "PostsInfo"

class SqlReader {
private:
	QSqlDatabase _dataBase;
public:
	SqlReader();
	~SqlReader();

	[[nodiscard]] Telegram::Message getMessage(int id);
private:
	[[nodiscard]] QString getDatabasePath() const;
};