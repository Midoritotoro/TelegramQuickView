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
#include <QLabel>
#include <QCloseEvent>

#include "UserDataManager.h"


class AuthenticationDialog: public QDialog
{
private:
    Q_OBJECT
    bool shakeSwitch = true;
    QFrame* firstAuthenticationStageFrame = nullptr, * secondAuthenticationStageFrame = nullptr;
    QPushButton* loginButton = nullptr;
    QLineEdit* apiHashLineEdit = nullptr, * apiIdLineEdit = nullptr, * phoneNumberLineEdit = nullptr, * telegramCodeLineEdit = nullptr;
    QToolButton* logInButton = nullptr;
    QStackedWidget* _stackedWidget = nullptr;
    UserDataManager* _userDataManager = nullptr;
    QLabel* _incorrectTelegramCredentialsLabel = nullptr, * _incorrectMobilePhoneLabel = nullptr,*_incorrectTelegramCodeLabel = nullptr;
    LPTelegramCredentials _telegramCredentials = nullptr;
public:
    AuthenticationDialog(QWidget* parent = nullptr);
public slots:
    void shake();
    void logInButton_clicked();
    void confirmMobilePhoneCodeButton_clicked();
    void backButton_clicked();
    void sendCodeAgainButton_clicked();
private:
    void vacillate();
protected:
    void closeEvent(QCloseEvent* event) override;
};