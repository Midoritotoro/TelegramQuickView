#pragma once

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>

class UserAuthenticationDialog : public QDialog
{
private:
	Q_OBJECT
    bool shakeSwitch;
public:
	UserAuthenticationDialog(QWidget* parent = nullptr);
public slots:
    void shake();
private:
	void vacillate();
protected:
	void closeEvent(QCloseEvent* event) override;
};
