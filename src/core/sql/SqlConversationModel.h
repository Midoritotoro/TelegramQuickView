#pragma once

#include <QSqlTableModel>

class SqlConversationModel : public QSqlTableModel
{
    Q_OBJECT
public:
    SqlConversationModel(QObject* parent = 0);
    Q_INVOKABLE void sendMessage(const QString& author, const QString& message, const QString& timestamp);
private:
    QString m_recipient;
};