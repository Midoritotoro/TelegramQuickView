﻿#pragma once



#include <QDialog>

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>

#include "UserDataManager.h"


class AuthenticationDialog : public QDialog
{
private:
    Q_OBJECT
    bool shakeSwitch = true, _skipFirstAuthenticationStage = false;
    QFrame* firstAuthenticationStageFrame = nullptr, * secondAuthenticationStageFrame = nullptr;
    QPushButton* loginButton = nullptr, * confirmCodeButton = nullptr, * sendCodeButton = nullptr;
    QLineEdit* apiHashLineEdit = nullptr, * apiIdLineEdit = nullptr, * phoneNumberLineEdit = nullptr, * telegramCodeLineEdit = nullptr;
    QToolButton* logInButton = nullptr;
    QStackedWidget* _stackedWidget = nullptr;
    UserDataManager* _userDataManager = nullptr;
    QLabel* _incorrectTelegramCredentialsLabel = nullptr, * _incorrectMobilePhoneLabel = nullptr, * _incorrectTelegramCodeLabel = nullptr;
    TelegramCredentials _telegramCredentials;
    TelegramAuthorizer _telegramAuthorizer;
    QTimer* timer = nullptr;
    QString _authorizationCode = "";
    int timeRemaining = 0;
    bool _isAuthCodeAccepted = false;
    bool _isTelegramCredentialsValid = false;
public:
    AuthenticationDialog(QWidget* parent = nullptr);

    void skipFirstAuthorizationStage();
    [[nodiscard]] bool isTelegramCredentialsValid();
    [[nodiscard]] bool isAuthCodeAccepted();
public Q_SLOTS:
    void shake();
    void logInButton_clicked();
    void confirmMobilePhoneCodeButton_clicked();
    void backButton_clicked();
    void sendCodeAgainButton_clicked();
    void updateSendCodeButtonText();
private:
    void vacillate();
    void updateAuthState();
protected:
    void closeEvent(QCloseEvent* event) override;
};