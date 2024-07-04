#include "UserAuthenticationDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>


UserAuthenticationDialog::UserAuthenticationDialog(QWidget* parent):
	QDialog(parent)
{
	setFixedSize(800, 600);
    QGridLayout* gridLayout = new QGridLayout(this);

    apiHashLineEdit = new QLineEdit();
    apiIdLineEdit = new QLineEdit();
    phoneNumberLineEdit = new QLineEdit();

    QLabel* apiHashLabel = new QLabel("Api hash: ");
    QLabel* apiIdLineLabel = new QLabel("Api id: ");
    QLabel* phoneNumberLineLabel = new QLabel("Номер телефона: ");

    apiHashLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    apiIdLineLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    phoneNumberLineLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QPushButton* logInButton = new QPushButton("Войти");

    gridLayout->setSpacing(10);
    gridLayout->setHorizontalSpacing(1);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(apiHashLabel, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(apiIdLineLabel, 1, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(phoneNumberLineLabel, 2, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);

    gridLayout->addWidget(apiHashLineEdit, 0, 1, 1, 1, Qt::AlignCenter);
    gridLayout->addWidget(apiIdLineEdit, 1, 1, 1, 1, Qt::AlignCenter);
    gridLayout->addWidget(phoneNumberLineEdit, 2, 1, 1, 1, Qt::AlignCenter);

    gridLayout->addWidget(logInButton, 3, 1, 1, 1, Qt::AlignCenter);

    connect(logInButton, &QPushButton::clicked, this, &UserAuthenticationDialog::logInButton_clicked);
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

void UserAuthenticationDialog::logInButton_clicked() {
    QString apiHash = apiHashLineEdit->text();
    QString apiId = apiIdLineEdit->text();
    QString phoneNumber = phoneNumberLineEdit->text();


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
