#include "AuthenticationDialog.h"

#include <QTimer>


AuthenticationDialog::AuthenticationDialog(QWidget* parent) :
    QDialog(parent)
{
    setFixedSize(720, 720);

    _stackedWidget = new QStackedWidget(this);
    _telegramCredentials = new TelegramCredentials();
    timer = new QTimer();

    setStyleSheet(QString::fromUtf8("*{\n"
        "font-family: centry gothic;\n"
        "font-size: 24px;\n"
        "}"));

    firstAuthenticationStageFrame = new QFrame();
    secondAuthenticationStageFrame = new QFrame();

    _incorrectTelegramCredentialsLabel = new QLabel(firstAuthenticationStageFrame);
    _incorrectMobilePhoneLabel = new QLabel(firstAuthenticationStageFrame);
    _incorrectTelegramCodeLabel = new QLabel(secondAuthenticationStageFrame);

    _incorrectTelegramCredentialsLabel->setWordWrap(true);
    _incorrectMobilePhoneLabel->setWordWrap(true);
    _incorrectTelegramCodeLabel->setWordWrap(true);

    _incorrectTelegramCredentialsLabel->setText("Введены неверные данные Telegram. Проверьте их корректность. ");
    _incorrectMobilePhoneLabel->setText("Ошибка при отправке кода Telegram. Проверьте корректность номера телефона. ");
    _incorrectTelegramCodeLabel->setText("Ошибка при входе в аккаунт Telegram. Проверьте корректность введённого кода. ");

    _incorrectTelegramCredentialsLabel->setStyleSheet("QLabel { color: red; font-size: 15px; }");
    _incorrectMobilePhoneLabel->setStyleSheet("QLabel { color: red; font-size: 15px; }");
    _incorrectTelegramCodeLabel->setStyleSheet("QLabel { color: red; font-size: 15px; }");

    _incorrectTelegramCredentialsLabel->setGeometry(QRect(50, 110, 450, 50));
    _incorrectMobilePhoneLabel->setGeometry(QRect(50, 110, 450, 50));
    _incorrectTelegramCodeLabel->setGeometry(QRect(50, 110, 450, 50));

    firstAuthenticationStageFrame->setGeometry(QRect(85, 120, 550, 550));
    firstAuthenticationStageFrame->setStyleSheet(QString::fromUtf8("QFrame{ \n"
        "background: #333;\n"
        "border-radius: 15px\n"
        "}"));

    firstAuthenticationStageFrame->setFrameShape(QFrame::Shape::StyledPanel);
    firstAuthenticationStageFrame->setFrameShadow(QFrame::Shadow::Raised);

    _stackedWidget->setGeometry(firstAuthenticationStageFrame->pos().x(), firstAuthenticationStageFrame->pos().y(), firstAuthenticationStageFrame->width(), firstAuthenticationStageFrame->height());

    loginButton = new QPushButton(firstAuthenticationStageFrame);
    loginButton->setGeometry(QRect(50, 410, 450, 80));
    loginButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
        "background: Wheat;\n"
        "border-radius: 15px;\n"
        "color: black;\n"
        "}\n"
        "QPushButton::hover { \n"
        "border-radius: 15px;\n"
        "background: NavajoWhite\n"
        "}"));

    apiHashLineEdit = new QLineEdit(firstAuthenticationStageFrame);
    apiHashLineEdit->setGeometry(QRect(50, 170, 450, 50));

    apiHashLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: white;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n"
        ""));

    apiIdLineEdit = new QLineEdit(firstAuthenticationStageFrame);
    apiIdLineEdit->setGeometry(QRect(50, 240, 450, 50));
    apiIdLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: white;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n"
        ""));

    phoneNumberLineEdit = new QLineEdit(firstAuthenticationStageFrame);
    phoneNumberLineEdit->setGeometry(QRect(50, 310, 450, 50));
    phoneNumberLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: white;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n"
        ""));

    logInButton = new QToolButton(this);

    logInButton->setGeometry(QRect(295, 60, 120, 120));
    logInButton->setStyleSheet(QString::fromUtf8("QToolButton{ \n"
        "        background: Wheat;\n"
        "        border-radius: 60px;\n"
        "        color: black;\n"
        "}\n"
        "QToolButton::hover { \n"
        "        background: NavajoWhite;\n"
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

    _stackedWidget->addWidget(firstAuthenticationStageFrame);

    secondAuthenticationStageFrame->setGeometry(QRect(85, 120, 550, 550));
    secondAuthenticationStageFrame->setStyleSheet(QString::fromUtf8("QFrame{ \n"
        "background: #333;\n"
        "border-radius: 15px\n"
        "}"));
    secondAuthenticationStageFrame->setFrameShape(QFrame::Shape::StyledPanel);
    secondAuthenticationStageFrame->setFrameShadow(QFrame::Shadow::Raised);

    sendCodeButton = new QPushButton(secondAuthenticationStageFrame);
    sendCodeButton->setObjectName("sendCodeButton");
    sendCodeButton->setGeometry(QRect(50, 250, 450, 70));
    sendCodeButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
        "        background: Wheat;\n"
        "        border-radius: 15px;\n"
        "        color: black;\n"
        "}\n"
        "QPushButton::hover { \n"
        "        border-radius: 15px;\n"
        "        background: NavajoWhite;\n"
        "}"));
    confirmCodeButton = new QPushButton(secondAuthenticationStageFrame);
    confirmCodeButton->setObjectName("confirmCodeButton");
    confirmCodeButton->setGeometry(QRect(50, 350, 450, 70));
    confirmCodeButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
        "        background: Wheat;\n"
        "        border-radius: 15px;\n"
        "        color: black;\n"
        "}\n"
        "QPushButton::hover { \n"
        "        border-radius: 15px;\n"
        "        background: NavajoWhite;\n"
        "}"));
    telegramCodeLineEdit = new QLineEdit(secondAuthenticationStageFrame);
    telegramCodeLineEdit->setObjectName("telegramCodeLineEdit");
    telegramCodeLineEdit->setGeometry(QRect(50, 150, 450, 50));
    telegramCodeLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit{\n"
        "        background: transparent;\n"
        "        border: none;\n"
        "        color: white;\n"
        "        border-bottom: 1px solid #717072;\n"
        "}"));
    QPushButton* backButton = new QPushButton(secondAuthenticationStageFrame);
    backButton->setObjectName("backButton");
    backButton->setGeometry(QRect(210, 450, 121, 41));
    backButton->setStyleSheet(QString::fromUtf8("QPushButton{\n"
        "        background: SandyBrown;\n"
        "        border-radius: 15px;\n"
        "        color: black;\n"
        "}\n"
        "QPushButton::hover { \n"
        "        border-radius: 15px;\n"
        "        background: #E49450;\n"
        "}"));

    sendCodeButton->setText("Отправить");
    confirmCodeButton->setText("Подтвердить");
    backButton->setText("Назад");
    telegramCodeLineEdit->setPlaceholderText("Код");

    _stackedWidget->addWidget(secondAuthenticationStageFrame);

    _incorrectTelegramCredentialsLabel->hide();
    _incorrectMobilePhoneLabel->hide();
    _incorrectTelegramCodeLabel->hide();

    connect(loginButton, &QPushButton::clicked, this, &AuthenticationDialog::logInButton_clicked);
    connect(backButton, &QPushButton::clicked, this, &AuthenticationDialog::backButton_clicked);
    connect(confirmCodeButton, &QPushButton::clicked, this, &AuthenticationDialog::confirmMobilePhoneCodeButton_clicked);
    connect(sendCodeButton, &QPushButton::clicked, this, &AuthenticationDialog::sendCodeAgainButton_clicked);
    connect(timer, &QTimer::timeout, this, &AuthenticationDialog::updateSendCodeButtonText);
}

void AuthenticationDialog::skipFirstAuthorizationStage() {
    backButton_clicked();
    logInButton_clicked();
}

void AuthenticationDialog::updateSendCodeButtonText() {
    timeRemaining--;

    if (timeRemaining >= 0) {
        QString text = QString("Осталось: %1 с").arg(timeRemaining);
        sendCodeButton->setText(text);
    }
    else {
        timer->stop();
        sendCodeButton->setEnabled(true);
        sendCodeButton->setText("Отправить");
        sendCodeButton->setToolTip("");
    }
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
    QTimer::singleShot(20, this, &AuthenticationDialog::shake);
}

void AuthenticationDialog::logInButton_clicked() {
    DWORD functionResult = 0;
    HANDLE thread = nullptr;
    bool local = false;


    _incorrectTelegramCredentialsLabel->hide();
    _incorrectMobilePhoneLabel->hide();
    _incorrectTelegramCodeLabel->hide();

    QString apiHash = apiHashLineEdit->text();
    QString apiId = apiIdLineEdit->text();
    QString phoneNumber = phoneNumberLineEdit->text();

    _telegramCredentials->apiHash = apiHash.toStdString();
    _telegramCredentials->apiId = apiId.toInt();
    _telegramCredentials->phoneNumber = phoneNumber.toStdString();

    if (!UserDataManager::setTelegramCredentials(_telegramCredentials)) {
        _incorrectTelegramCredentialsLabel->show();
        UserDataManager::clearTelegramCredentials();
        apiHashLineEdit->clear();
        apiIdLineEdit->clear();
        phoneNumberLineEdit->clear();
        shake();
        return;
    }

    thread = CreateThread(NULL, 0, &UserDataManager::isTelegramCredentialsValid, NULL, 0, NULL);
    _Analysis_assume_(thread != NULL);

    WaitForSingleObject(thread, INFINITE);

    if (!GetExitCodeThread(thread, &functionResult))
        return;

    CloseHandle(thread);

    if (functionResult == 0) {
        _incorrectTelegramCredentialsLabel->show();
        qDebug() << "isTelegramCredentialsValid = 0";
        //UserDataManager::clearTelegramCredentials();
        apiHashLineEdit->clear();
        apiIdLineEdit->clear();
        phoneNumberLineEdit->clear();
        shake();
        return;
    }

    thread = CreateThread(NULL, 0, &TelegramAuthorizationChecker::sendTelegramCode, _telegramCredentials, 0, NULL);
    _Analysis_assume_(thread != NULL);
    WaitForSingleObject(thread, INFINITE);

    if (!GetExitCodeThread(thread, &functionResult))
        return;
    CloseHandle(thread);
    functionResult = TelegramAuthorizationChecker::sendTelegramCode(_telegramCredentials);

    if (!functionResult) {
        _incorrectMobilePhoneLabel->show();
        shake();
        return;
    }

    timeRemaining = 180;
    timer->start(1000);
    sendCodeButton->setEnabled(false);
    sendCodeButton->setToolTip("Кнопка неактивна в течение 180 секунд по причине ограничений Telegram. ");
    sendCodeButton->setText(QString("Осталось: %1 с").arg(timeRemaining));
    _stackedWidget->setCurrentIndex(1);

}

void AuthenticationDialog::confirmMobilePhoneCodeButton_clicked() {
    QString mobilePhoneCode = telegramCodeLineEdit->text();

    if (mobilePhoneCode.length() < 5) {
        _incorrectTelegramCodeLabel->show();
        telegramCodeLineEdit->clear();
        shake();
        return;
    }

    UserDataManager::setPhoneNumberCode(mobilePhoneCode);
    if (Py_IsInitialized())
        Py_InitializeEx(0);
    if (!UserDataManager::isTelegramPhoneNumberCodeValid()) {
        _incorrectTelegramCodeLabel->show();
        telegramCodeLineEdit->clear();
        shake();
        Py_FinalizeEx();
        return;
    }
    if (Py_IsInitialized())
        Py_FinalizeEx();
    close();
}

void AuthenticationDialog::backButton_clicked() {

    _telegramCredentials = UserDataManager::getTelegramCredentials();
    apiHashLineEdit->setText(_telegramCredentials->apiHash.c_str());
    apiIdLineEdit->setText(QString::number(_telegramCredentials->apiId));
    phoneNumberLineEdit->setText(_telegramCredentials->phoneNumber.c_str());
    _stackedWidget->setCurrentIndex(0);
}

void AuthenticationDialog::sendCodeAgainButton_clicked() {
    QFile file("TelegramQuickView.session");
    file.remove();
    DWORD functionResult = 0;

    _telegramCredentials = UserDataManager::getTelegramCredentials();

    HANDLE thread = CreateThread(NULL, 0, &TelegramAuthorizationChecker::sendTelegramCode, _telegramCredentials, 0, NULL);
    _Analysis_assume_(thread != NULL);
    WaitForSingleObject(thread, INFINITE);

    if (!GetExitCodeThread(thread, &functionResult))
        return;
    CloseHandle(thread);

    if (!functionResult) {
        shake();
        return;
    }

    timeRemaining = 180;
    timer->start(1000);
    sendCodeButton->setEnabled(false);
    sendCodeButton->setToolTip("Кнопка неактивна в течение 180 секунд по причине ограничений Telegram. ");
    sendCodeButton->setText(QString("Осталось: %1 с").arg(timeRemaining));
}

void AuthenticationDialog::closeEvent(QCloseEvent* event) {
    if (UserDataManager::isTelegramPhoneNumberCodeValid() == false || UserDataManager::isTelegramCredentialsValid() == false) {
        event->ignore();
        shake();
    }
    else {
        event->accept();
    }
}

void AuthenticationDialog::vacillate()
{
    QPoint offset(5, 10);
    move(((shakeSwitch) ? pos() + offset : pos() - offset));
    shakeSwitch = !shakeSwitch;
}