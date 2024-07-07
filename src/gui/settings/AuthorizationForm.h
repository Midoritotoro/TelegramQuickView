#pragma once

#include <QVariant>
#include <QIcon>
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>


class AuthenticationDialog: public QDialog
{
private:
    bool shakeSwitch = false;
    QFrame* firstAuthenticationStageFrame = nullptr, * secondAuthenticationStageFrame = nullptr;
    QPushButton* loginButton = nullptr;
    QLineEdit* apiHashLineEdit = nullptr, *apiIdLineEdit = nullptr, *phoneNumberLineEdit = nullptr;
    QToolButton* logInButton = nullptr;
    QStackedWidget* _stackedWidget = nullptr;
public:
    AuthenticationDialog(QWidget* parent = nullptr);
public slots:
    void shake();
    void logInButton_clicked();
    void confirmMobilePhoneCodeButton_clicked();
    void backToLogInScreenButton_clicked();
    void sendCodeAgainButton_clicked();
private:
    void vacillate();
protected:
    void closeEvent(QCloseEvent* event) override;
};

