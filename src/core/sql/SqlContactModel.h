#pragma once

#include <QSqlQueryModel>

class SqlContactModel : public QSqlQueryModel
{
public:
    SqlContactModel(QObject* parent = 0);
};
