#pragma once

#include <QDialog>

class AuthenticationDialog : public QDialog
{
private:
	Q_OBJECT
public:
	AuthenticationDialog(QWidget* parent = nullptr);
};
