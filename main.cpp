#include <QGuiApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtCore>
#include <QtQml>

#include "src/core/sql/SqlConversationModel.h"
#include <Windows.h>

static void connectToDatabase()
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

int main(int argc, char* argv[])
{
    AllocConsole();
    QGuiApplication app(argc, argv);

    qmlRegisterType<SqlConversationModel>("sql.SqlConversationModel", 1, 0, "SqlConversationModel");

    connectToDatabase();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("../../src/gui/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
