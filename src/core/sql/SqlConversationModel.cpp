#include "SqlConversationModel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

static const char* conversationsTableName = "Conversations";

static void createTable()
{
    if (QSqlDatabase::database().tables().contains(conversationsTableName)) {
        return;
    }

    QSqlQuery query;
    if (!query.exec(
        "CREATE TABLE IF NOT EXISTS 'Conversations' ("
        "'author' TEXT NOT NULL,"
        "'timestamp' TEXT NOT NULL,"
        "'message' TEXT NOT NULL,"
        "FOREIGN KEY('author') REFERENCES Contacts ( name ),"
        ")")) {
        return;
    }
}

SqlConversationModel::SqlConversationModel(QObject* parent) :
    QSqlTableModel(parent)
{
    createTable();
    setTable(conversationsTableName);
    setSort(2, Qt::DescendingOrder);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}


void SqlConversationModel::sendMessage(const QString& recipient, const QString& message)
{
    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    QSqlRecord newRecord = record();
    newRecord.setValue("author", "Me");
    newRecord.setValue("recipient", recipient);
    newRecord.setValue("timestamp", timestamp);
    newRecord.setValue("message", message);
    if (!insertRecord(rowCount(), newRecord)) {
        return;
    }

    submitAll();
}