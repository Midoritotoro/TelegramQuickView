#include "AuthorizationForm.h"

AuthenticationDialog::AuthenticationDialog(QWidget* parent):
    QDialog(parent)
{
    setFixedSize(720, 720);

    setStyleSheet(QString::fromUtf8("*{\n"
        "font-family: centry gothic;\n"
        "font-size: 24px;\n"
        "}"));

    frame = new QFrame(this);
    frame->setObjectName("frame");
    frame->setGeometry(QRect(90, 120, 550, 550));
    frame->setStyleSheet(QString::fromUtf8("QFrame{ \n"
        "background: #333;\n"
        "border-radius: 15px\n"
        "}"));

    frame->setFrameShape(QFrame::Shape::StyledPanel);
    frame->setFrameShadow(QFrame::Shadow::Raised);

    loginButton = new QPushButton(frame);
    loginButton->setObjectName("loginButton");
    loginButton->setGeometry(QRect(50, 450, 450, 80));
    loginButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
        "background: red;\n"
        "border-radius: 15px;\n"
        "color: white;\n"
        "}\n"
        "QPushButton::hover { \n"
        "color: red;\n"
        "border-radius: 15px;\n"
        "background: #49ebff\n"
        "}"));

    apiHashLineEdit = new QLineEdit(frame);
    apiHashLineEdit->setObjectName("lineEdit");
    apiHashLineEdit->setGeometry(QRect(50, 130, 450, 24));

    apiHashLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: #717072;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n"
        ""));

    apiIdLineEdit = new QLineEdit(frame);
    apiIdLineEdit->setObjectName("lineEdit_2");
    apiIdLineEdit->setGeometry(QRect(50, 240, 450, 24));
    apiIdLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: #717072;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n"
        ""));

    phoneNumberLineEdit = new QLineEdit(frame);
    phoneNumberLineEdit->setObjectName("lineEdit_3");
    phoneNumberLineEdit->setGeometry(QRect(50, 360, 450, 24));
    phoneNumberLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: #717072;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n"
        ""));

    logInButton = new QToolButton(this);
    logInButton->setObjectName("toolButton");
    logInButton->setGeometry(QRect(310, 60, 120, 120));
    logInButton->setStyleSheet(QString::fromUtf8("QToolButton{ \n"
        "background: red;\n"
        "border-radius: 60px;\n"
        "}"));

    QIcon icon;
    icon.addFile(QString::fromUtf8("../../assets/images/auth.png"), QSize(), QIcon::Normal, QIcon::Off);
    logInButton->setIcon(icon);
    logInButton->setIconSize(QSize(48, 48));

    setWindowTitle("Аутентификация");
    loginButton->setText("Войти");

    apiHashLineEdit->setPlaceholderText("Api Hash");
    apiIdLineEdit->setPlaceholderText("Api Id");
    phoneNumberLineEdit->setPlaceholderText("Телефонный номер");

    //QMetaObject::connectSlotsByName(this);
    connect(logInButton, &QToolButton::clicked, this, &AuthenticationDialog::logInButton_clicked);
}


void AuthenticationDialog::shake()
{
    static int numTimesCalled = 0;
    numTimesCalled++;

    if (numTimesCalled == 9) {
        numTimesCalled = 0;
        return;
    }

    vacillate();
    // QTimer::singleShot(20, this, SLOT(shake()));
}

void AuthenticationDialog::logInButton_clicked() {
   /* _incorrentTelegramCredentialsLabel->hide();
    _incorrentMobilePhoneLabel->hide();
    _incorrectTelegramCodeLabel->hide();

    QString apiHash = apiHashLineEdit->text();
    QString apiId = apiIdLineEdit->text();
    QString phoneNumber = phoneNumberLineEdit->text();

    _userDataManager->setTelegramCredentials(apiHash, phoneNumber, apiId);
    _mobilePhoneNumberLabel->setText(phoneNumberLineEdit->text());
    if (!_userDataManager->isTelegramCredentialsValid()) {
        _incorrentTelegramCredentialsLabel->show();
        _userDataManager->clearTelegramCredentials();
        shake();
        return;
    }
    TelegramAuthorizationChecker* telegramAuthorizationChecker = new TelegramAuthorizationChecker();
    bool isCodeSended = telegramAuthorizationChecker->sendTelegramCode(apiHash.toStdString().c_str(), phoneNumber.toStdString().c_str(), apiId.toInt(), _userDataManager->getUserSettingsPath().toStdString().c_str());

    if (!isCodeSended) {
        _incorrentMobilePhoneLabel->show();
        shake();
        return;
    }
    _stackedLayout->setCurrentIndex(1);*/
}

void AuthenticationDialog::confirmMobilePhoneCodeButton_clicked() {
   /* QString mobilePhoneCode = _mobilePhoneCodeLineEdit->text();
    _userDataManager->setPhoneNumberCode(mobilePhoneCode);

    if (!_userDataManager->isTelegramPhoneNumberCodeValid()) {
        _incorrectTelegramCodeLabel->show();
        _mobilePhoneCodeLineEdit->clear();
        shake();
        return;
    }
    close();*/
}

void AuthenticationDialog::backToLogInScreenButton_clicked() {
    //QStringList telegramCredentialsList = _userDataManager->getTelegramCredentials();
    //_stackedLayout->setCurrentIndex(0);
    //apiHashLineEdit->setText(telegramCredentialsList.at(0));
    //apiIdLineEdit->setText(telegramCredentialsList.at(1));
    //phoneNumberLineEdit->setText(telegramCredentialsList.at(2));
}

void AuthenticationDialog::sendCodeAgainButton_clicked() {
   /* QFile file("TelegramQuickView.session");
    qDebug() << file.remove();
    QStringList telegramCredentialsList = _userDataManager->getTelegramCredentials();


    TelegramAuthorizationChecker* telegramAuthorizationChecker = new TelegramAuthorizationChecker();
    bool isCodeSended = telegramAuthorizationChecker->sendTelegramCode(telegramCredentialsList.at(0).toStdString().c_str(), telegramCredentialsList.at(1).toStdString().c_str(), telegramCredentialsList.at(2).toInt(), _userDataManager->getUserSettingsPath().toStdString().c_str());
    if (!isCodeSended) {
        shake();
        return;
    }*/
}

void AuthenticationDialog::closeEvent(QCloseEvent* event) {
   /* if (!_userDataManager->isTelegramPhoneNumberCodeValid()) {
        event->ignore();
        shake();
    }
    else {
        event->accept();
    }*/
}

void AuthenticationDialog::vacillate()
{/*
    QPoint offset(5, 10);
    move(((shakeSwitch) ? pos() + offset : pos() - offset));
    shakeSwitch = !shakeSwitch;*/
}
