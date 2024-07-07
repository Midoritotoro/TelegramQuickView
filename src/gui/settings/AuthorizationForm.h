#pragma once

#include <QVariant>
#include <QIcon>
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
private:
    QFrame* frame;
    QPushButton* loginButton;
    QLineEdit* lineEdit;
    QLineEdit* lineEdit_2;
    QLineEdit* lineEdit_3;
    QToolButton* toolButton;
public:
    void setupUi(QDialog* Dialog);
    void retranslateUi(QDialog* Dialog);
};

namespace Ui {
    class Dialog : public Ui_Dialog {};
}

QT_END_NAMESPACE
