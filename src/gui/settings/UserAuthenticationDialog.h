#pragma once

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QLineEdit>
#include <QStackedLayout>
#include <QLabel>
#include "UserDataManager.h"


class UserAuthenticationDialog : public QDialog
{
private:
	Q_OBJECT
    bool shakeSwitch = true;
	QLineEdit* apiHashLineEdit = nullptr, * apiIdLineEdit = nullptr, * phoneNumberLineEdit = nullptr, *_mobilePhoneCodeLineEdit = nullptr;
	UserDataManager* _userDataManager = nullptr;
	QStackedLayout* _stackedLayout = nullptr;
	QLabel* _mobilePhoneNumberLabel = nullptr;
	QLabel* _incorrentTelegramCredentialsLabel = nullptr, * _incorrentMobilePhoneLabel = nullptr, *_incorrectTelegramCodeLabel = nullptr;
public:
	UserAuthenticationDialog(QWidget* parent = nullptr);
public slots:
    void shake();
	void logInButton_clicked();
	void confirmMobilePhoneCodeButton_clicked();
	void backToLogInScreenButton_clicked();
private:
	void vacillate();
protected:
	void closeEvent(QCloseEvent* event) override;

};
