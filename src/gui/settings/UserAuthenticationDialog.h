#pragma once

#include <QDialog>

class UserAuthenticationDialog : public QDialog
{
private:
	Q_OBJECT
public:
	UserAuthenticationDialog(QWidget* parent = nullptr);
};
