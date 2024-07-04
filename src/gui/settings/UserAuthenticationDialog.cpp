#include "UserAuthenticationDialog.h"

UserAuthenticationDialog::UserAuthenticationDialog(QWidget* parent):
	QDialog(parent)
{
	setFixedSize(800, 600);

}

void UserAuthenticationDialog::shake()
{
    static int numTimesCalled = 0;
    numTimesCalled++;

    if (numTimesCalled == 9) {
        numTimesCalled = 0;
        return;
    }

    vacillate();
    QTimer::singleShot(20, this, SLOT(shake()));
}

void UserAuthenticationDialog::closeEvent(QCloseEvent* event) {
	event->ignore();
	shake();
	//QDialog::closeEvent(event);
}

void UserAuthenticationDialog::vacillate()
{
    QPoint offset(5, 10);
    move(((shakeSwitch) ? pos() + offset : pos() - offset));
    shakeSwitch = !shakeSwitch;
}