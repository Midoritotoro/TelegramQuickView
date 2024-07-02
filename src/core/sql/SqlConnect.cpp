#include "SqlConnect.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QtCore>

void connectToDatabase()
{
    QSqlDatabase database = QSqlDatabase::database();

    if (!database.isValid()) {
        database = QSqlDatabase::addDatabase("QSQLITE");
        if (!database.isValid())
            return;
    }

    const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!writeDir.mkpath("."))
        return;

    const QString fileName = writeDir.absolutePath() + "/chat-database.sqlite3";
    database.setDatabaseName(fileName);

    if (!database.open())
        QFile::remove(fileName);
}
