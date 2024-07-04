#pragma once

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>

class UserAuthenticationDialog : public QDialog
{
private:
	Q_OBJECT
    bool shakeSwitch;
	QLineEdit* apiHashLineEdit = nullptr, * apiIdLineEdit = nullptr, * phoneNumberLineEdit = nullptr;
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
