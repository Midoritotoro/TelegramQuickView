#pragma once

#include <QSqlTableModel>

class SqlConversationModel : public QSqlTableModel
{
    Q_OBJECT
public:
    SqlConversationModel(QObject* parent = 0);
    Q_INVOKABLE void sendMessage(const QString& recipient, const QString& message);
private:
    QString m_recipient;
};