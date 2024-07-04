#include "UserAuthenticationDialog.h"

#include <QGridLayout>
#include <QPushButton>


UserAuthenticationDialog::UserAuthenticationDialog(QWidget* parent):
	QDialog(parent)
{
	setFixedSize(800, 600);

    _stackedLayout = new QStackedLayout(this);

    QWidget* firstAuthenticationStageWidget = new QWidget();
    QWidget* secondAuthenticationStageWidget = new QWidget();

    _userDataManager = new UserDataManager();
    QGridLayout* firstAuthenticationStageGridLayout = new QGridLayout(firstAuthenticationStageWidget);
    QGridLayout* secondAuthenticationStageGridLayout = new QGridLayout(secondAuthenticationStageWidget);

    apiHashLineEdit = new QLineEdit();
    apiIdLineEdit = new QLineEdit();
    phoneNumberLineEdit = new QLineEdit();
    _mobilePhoneCodeLineEdit = new QLineEdit();

    QLabel* apiHashLabel = new QLabel("Api hash: ");
    QLabel* apiIdLabel = new QLabel("Api id: ");
    QLabel* phoneNumberLabel = new QLabel("Номер телефона: ");
    QLabel* mobilePhoneCodeLabel = new QLabel("Код подтверждения: ");

    _mobilePhoneNumberLabel = new QLabel();

    apiHashLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    apiIdLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    phoneNumberLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QPushButton* logInButton = new QPushButton("Войти");
    QPushButton* confirmMobilePhoneCodeButton = new QPushButton("Подтвердить");

    firstAuthenticationStageGridLayout->setSpacing(10);
    firstAuthenticationStageGridLayout->setHorizontalSpacing(1);
    firstAuthenticationStageGridLayout->setContentsMargins(0, 0, 0, 0);
    firstAuthenticationStageGridLayout->setAlignment(Qt::AlignCenter);

    firstAuthenticationStageGridLayout->addWidget(apiHashLabel, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    firstAuthenticationStageGridLayout->addWidget(apiIdLabel, 1, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    firstAuthenticationStageGridLayout->addWidget(phoneNumberLabel, 2, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);

    firstAuthenticationStageGridLayout->addWidget(apiHashLineEdit, 0, 1, 1, 1, Qt::AlignCenter);
    firstAuthenticationStageGridLayout->addWidget(apiIdLineEdit, 1, 1, 1, 1, Qt::AlignCenter);
    firstAuthenticationStageGridLayout->addWidget(phoneNumberLineEdit, 2, 1, 1, 1, Qt::AlignCenter);

    firstAuthenticationStageGridLayout->addWidget(logInButton, 3, 1, 1, 1, Qt::AlignCenter);

    secondAuthenticationStageGridLayout->addWidget(mobilePhoneCodeLabel, 0, 0, 1, 1, Qt::AlignVCenter | Qt::AlignTop);
    secondAuthenticationStageGridLayout->addWidget(mobilePhoneCodeLabel, 1, 0, 1, 1, Qt::AlignVCenter | Qt::AlignRight);
    secondAuthenticationStageGridLayout->addWidget(_mobilePhoneCodeLineEdit, 1, 1, 1, 1, Qt::AlignCenter);
    secondAuthenticationStageGridLayout->addWidget(confirmMobilePhoneCodeButton, 2, 1, 1, 1, Qt::AlignCenter);

    _stackedLayout->addWidget(firstAuthenticationStageWidget);
    _stackedLayout->addWidget(secondAuthenticationStageWidget);


    connect(logInButton, &QPushButton::clicked, this, &UserAuthenticationDialog::logInButton_clicked);
    connect(confirmMobilePhoneCodeButton, &QPushButton::clicked, this, &UserAuthenticationDialog::confirmMobilePhoneCode_clicked);
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

    _userDataManager->authorize(apiHash, phoneNumber, apiId);
    _mobilePhoneNumberLabel->setText(phoneNumberLineEdit->text());
    _stackedLayout->setCurrentIndex(1);
}

void UserAuthenticationDialog::confirmMobilePhoneCode_clicked() {
    QString mobilePhoneCode = _mobilePhoneCodeLineEdit->text();

}

void UserAuthenticationDialog::closeEvent(QCloseEvent* event) {
    if (!_userDataManager->isUserAuthorized()) {
        event->ignore();
        shake();
    }
    else {
        event->accept();
    }
}

void UserAuthenticationDialog::vacillate()
{
    QPoint offset(5, 10);
    move(((shakeSwitch) ? pos() + offset : pos() - offset));
    shakeSwitch = !shakeSwitch;
}
