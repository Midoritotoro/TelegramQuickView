#include <QGuiApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtCore>
#include <QtQml>

#include "src/core/sql/SqlContactModel.h"
#include "src/core/sql/SqlConversationModel.h"
#include <Windows.h>

static void connectToDatabase()
{
    QSqlDatabase database = QSqlDatabase::database();
    if (!database.isValid()) {
        database = QSqlDatabase::addDatabase("QSQLITE");
        if (!database.isValid())
            qFatal("Cannot add database: %s", qPrintable(database.lastError().text()));
    }

    const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable directory at %s", qPrintable(writeDir.absolutePath()));

    // Ensure that we have a writable location on all devices.
    const QString fileName = writeDir.absolutePath() + "/chat-database.sqlite3";
    // When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
    database.setDatabaseName(fileName);
    if (!database.open()) {
        qFatal("Cannot open database: %s", qPrintable(database.lastError().text()));
        QFile::remove(fileName);
    }
}

int main(int argc, char* argv[])
{
    AllocConsole();
    QGuiApplication app(argc, argv);

    qmlRegisterType<SqlContactModel>("sql.SqlContactModel", 1, 0, "SqlContactModel");
    qmlRegisterType<SqlConversationModel>("sql.SqlConversationModel", 1, 0, "SqlConversationModel");

    connectToDatabase();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("../../../src/gui/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
