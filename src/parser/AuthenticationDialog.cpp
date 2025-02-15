﻿#include "AuthenticationDialog.h"

#include "../ui/style/StyleCore.h"

#include <QFontMetrics>
#include <QApplication>

#include <QScreen>
#include <QCoreApplication>

#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QMargins>

#include <QThread>

namespace {
    QSize textSize(const QString& text, const QFontMetrics& metrics) {
        return metrics.size(0, text);
    }

    QSize textSize(const QString& text, const QFont& font) {
        return text.isEmpty() ? QSize{} : textSize(text, QFontMetrics(font));
    }

    const QString incorrectCredentialsMessage = "Введены неверные данные Telegram. Проверьте их корректность.";
    const QString incorrectPhoneMessage = "Ошибка при отправке кода Telegram. Проверьте корректность номера телефона.";
    const QString incorrectCodeMessage = "Ошибка при входе в аккаунт Telegram. Проверьте корректность введённого кода.";
    const QString tooManyRequestsMessage = "Слишком много запросов, повторите через %1 секунд";

    const QString buttonStyle = "QPushButton{\n"
        "background: Wheat;\n"
        "color: black;\n"
        "border-radius: 10px;\n"
        "}\n"
        "QPushButton::hover { \n"
        "background: NavajoWhite;\n"
        "}";

    const QString backButtonStyle = "QPushButton{\n"
        "background: rgb(128, 128, 128);\n"
        "color: black;\n"
        "border-radius: 10px;\n"
        "}\n"
        "QPushButton::hover { \n"
        "background: rgb(148, 148, 148);\n"
        "}";

    const QString lineEditStyle = "QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: white;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n";

    constexpr QMargins authButtonsMargins = { 10, 0, 10, 0 };
}


AuthenticationDialog::AuthenticationDialog(QWidget* parent) :
    QDialog(parent)
    , _currentErrorCode(Telegram::ErrorCodes::OK)
{
    setWindowTitle(PROJECT_NAME);

    _pathToBackgroundImage = ":/assets/images/main_screen.jpg";
    _background = QPixmap(_pathToBackgroundImage);

    setStyleSheet(
        "*{\n"
        "font-family: centry gothic;\n"
        "font-size: 16px;\n"
        "}"
    );

    _confirmCredentialsButton = new QPushButton(this);
    _confirmAuthCodeButton = new QPushButton(this);
    _backToFirstFrameButton = new QPushButton(this);
    _sendCodeAgainButton = new QPushButton(this);

    _confirmCredentialsButton->setStyleSheet(buttonStyle);
    _confirmAuthCodeButton->setStyleSheet(buttonStyle);
    _backToFirstFrameButton->setStyleSheet(backButtonStyle);
    _sendCodeAgainButton->setStyleSheet(buttonStyle);

    _confirmCredentialsButton->setText("Войти");
    _confirmAuthCodeButton->setText("Подтвердить");
    _backToFirstFrameButton->setText("Назад");
    _sendCodeAgainButton->setText("Отправить");

    _apiHashLineEdit = new QLineEdit(this);
    _apiHashLineEdit->setStyleSheet(lineEditStyle);

    _apiIdLineEdit = new QLineEdit(this);
    _apiIdLineEdit->setStyleSheet(lineEditStyle);

    _phoneNumberLineEdit = new QLineEdit(this);
    _phoneNumberLineEdit->setStyleSheet(lineEditStyle);

    _telegramCodeLineEdit = new QLineEdit(this);
    _telegramCodeLineEdit->setStyleSheet(lineEditStyle);

    _telegramCodeLineEdit->setPlaceholderText("Код");
    _apiHashLineEdit->setPlaceholderText("Api Hash");
    _apiIdLineEdit->setPlaceholderText("Api Id");
    _phoneNumberLineEdit->setPlaceholderText("Телефонный номер");

    _apiHashLineEdit->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    _phoneNumberLineEdit->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    _apiIdLineEdit->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    _telegramCodeLineEdit->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    _apiHashLineEdit->setTextMargins(0, 0, 0, 5);
    _apiIdLineEdit->setTextMargins(0, 0, 0, 5);
    _phoneNumberLineEdit->setTextMargins(0, 0, 0, 5);
    _telegramCodeLineEdit->setTextMargins(0, 0, 0, 5);

    connect(_confirmCredentialsButton, &QPushButton::clicked, [this]() {
        if (_skipFirstAuthenticationStage) {
            toSecondFrame();
            return;
        }

        const auto apiHash = _apiHashLineEdit->text().toStdString();
        const auto apiId = _apiIdLineEdit->text().toInt();
        const auto phoneNumber = _phoneNumberLineEdit->text().toStdString();

        TelegramCredentials credentials;

        credentials.apiHash = apiHash;
        credentials.apiId = apiId;
        credentials.phoneNumber = phoneNumber.length() != 0 ?
            phoneNumber.at(0) == '+' ?
            phoneNumber : "+" + phoneNumber : "";

        if (credentials.isEmpty())
            return;

        emit credentialsAccepted(credentials);
        toSecondFrame();
        });

    connect(_confirmAuthCodeButton, &QPushButton::clicked, [this]() {
        const QString& mobilePhoneCode = _telegramCodeLineEdit->text();

        if (mobilePhoneCode.length() < 5) {
            _telegramCodeLineEdit->clear();
            shake();
            return;
        }

        emit authCodeAccepted(mobilePhoneCode);
        });

    connect(_backToFirstFrameButton, &QPushButton::clicked, [this]() {
        toFirstFrame();
        });

    connect(_sendCodeAgainButton, &QPushButton::clicked, [this]() {
        emit needSendCodeAgain();
        });

    toFirstFrame();

    _apiHashLineEdit->setText("019edf3f20c8460b741fb94114e6fec0");
    _apiIdLineEdit->setText("13711370");
    _phoneNumberLineEdit->setText("+375292384917");
}

void AuthenticationDialog::setSleepSeconds(int seconds) {
    _sleepSeconds = seconds;
}

void AuthenticationDialog::setErrorCode(Telegram::ErrorCodes code) {
    _currentErrorCode = code;
}

void AuthenticationDialog::setCloseAbility(bool closeAbility) {
    _canClose = closeAbility;
    if (_canClose)
        close();
}

void AuthenticationDialog::toSecondFrame() {
    hideWidgets();
    _confirmAuthCodeButton->show();
    _backToFirstFrameButton->show();
    _telegramCodeLineEdit->show();
    _sendCodeAgainButton->show();
}

void AuthenticationDialog::toFirstFrame() {
    hideWidgets();
    _confirmCredentialsButton->show();
    _apiHashLineEdit->show();
    _apiIdLineEdit->show();
    _phoneNumberLineEdit->show();
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

void AuthenticationDialog::resizeEvent(QResizeEvent* event) {
    if (_background.size() == size())
        return;

    _background = _background.scaled(style::getMinimumSizeWithAspectRatio(_background.size(), _background.size().width() / 4.),
                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
    resize(_background.size());

    updateWidgetsGeometry();
}

void AuthenticationDialog::paintEvent(QPaintEvent* event) {
    if (_background.isNull())
        return;

    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, _background);

    const auto widgetsWidth = width() / 3.;
    const auto widgetsHeight = height() / 12.;

    const auto authFrameWidth = widgetsWidth * 1.2;
    const auto authFrameHeight = widgetsHeight * 5.5;

    painter.setOpacity(0.8);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    QRect rect((width() - authFrameWidth) / 2., (height() - authFrameHeight) / 2., authFrameWidth, authFrameHeight);

    style::RoundCorners(painter, size(), 10);

    painter.drawRect(rect);
    paintErrorMessage(painter);
}

void AuthenticationDialog::closeEvent(QCloseEvent* event) {
    if (_canClose)
        QWidget::closeEvent(event);
    else
        shake();
}

void AuthenticationDialog::vacillate()
{
    QPoint offset(5, 10);
    move(((_shakeSwitch) ? pos() + offset : pos() - offset));

    _shakeSwitch = !_shakeSwitch;
}

void AuthenticationDialog::hideWidgets() {
    foreach(auto item, children())
        if (qobject_cast<QWidget*>(item))
            qobject_cast<QWidget*>(item)->hide();
}


void AuthenticationDialog::paintErrorMessage(QPainter& painter) {
    QString text;

    switch (_currentErrorCode) {
    case Telegram::ErrorCodes::OK:
        break;

    case Telegram::ErrorCodes::IncorrectApiHashOrId:
        text = incorrectCredentialsMessage;
        break;

    case Telegram::ErrorCodes::IncorrectAuthCode :
        text = incorrectCodeMessage;
        break;

    case Telegram::ErrorCodes::IncorrectPhoneNumber:
        text = incorrectPhoneMessage;
        break;

    case Telegram::ErrorCodes::TooManyRequests:
        text = tooManyRequestsMessage.arg(_sleepSeconds);
        break;
    }

    QFont font("Arial", 16);

    const auto errorTextSize = textSize(text, font);

    QRect attachmentsCountTextRect(QPoint(), errorTextSize);
    attachmentsCountTextRect.moveCenter(rect().center());

    painter.setOpacity(1.0);
    painter.setFont(font);
    painter.drawText(attachmentsCountTextRect, Qt::AlignCenter, text);
}

void AuthenticationDialog::updateWidgetsGeometry() {
    const auto widgetsWidth = width() / 3.;
    const auto widgetsHeight = height() / 12.;

    const auto centerX = (width() - widgetsWidth) / 2.;

    _confirmCredentialsButton->resize(widgetsWidth / 2., widgetsHeight);
    _confirmAuthCodeButton->resize(widgetsWidth / 2., widgetsHeight);
    _backToFirstFrameButton->resize(widgetsWidth / 2., widgetsHeight);
    _sendCodeAgainButton->resize(widgetsWidth / 2., widgetsHeight);

    _apiHashLineEdit->resize(widgetsWidth, widgetsHeight);
    _apiIdLineEdit->resize(widgetsWidth, widgetsHeight);
    _phoneNumberLineEdit->resize(widgetsWidth, widgetsHeight);
    _telegramCodeLineEdit->resize(widgetsWidth, widgetsHeight);

    _confirmCredentialsButton->move((width() - widgetsWidth / 2.) / 2., (height() + widgetsHeight * 2.5) / 2.);
    _confirmAuthCodeButton->move((width() - widgetsWidth / 2.) / 2., (height() - widgetsHeight / 1.5) / 2.);

    _phoneNumberLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2. - widgetsHeight * 2);
    _apiIdLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2. - widgetsHeight);
    _apiHashLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2.);

    _sendCodeAgainButton->move((centerX)-authButtonsMargins.left(), (height() + widgetsHeight * 2.) / 2.);
    _backToFirstFrameButton->move((centerX + widgetsWidth / 2.) + authButtonsMargins.right(), (height() + widgetsHeight * 2.) / 2.);

    _telegramCodeLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2. - widgetsHeight * 2);
}