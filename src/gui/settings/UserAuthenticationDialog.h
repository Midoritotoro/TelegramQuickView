#pragma once

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QLineEdit>
#include "UserDataManager.h"


class UserAuthenticationDialog : public QDialog
{
private:
	Q_OBJECT
    bool shakeSwitch = true;
	QLineEdit* apiHashLineEdit = nullptr, * apiIdLineEdit = nullptr, * phoneNumberLineEdit = nullptr;
	UserDataManager* _userDataManager = nullptr;
public:
	UserAuthenticationDialog(QWidget* parent = nullptr);
public slots:
    void shake();
	void logInButton_clicked();
private:
	void vacillate();
protected:
	void closeEvent(QCloseEvent* event) override;

};
