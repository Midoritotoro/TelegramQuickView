#include "AuthenticationDialog.h"

#include <QTimer>
#include <QFontMetrics>
#include <QApplication>
#include <QScreen>


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

    const QString buttonStyle = "QPushButton{\n"
            "background: Wheat;\n"
            "border-radius: 15px;\n"
            "color: black;\n"
        "}\n"
        "QPushButton::hover { \n"
            "background: NavajoWhite;\n"
        "}";

    const QString lineEditStyle = "QLineEdit{\n"
        "background: transparent;\n"
        "border: none;\n"
        "color: white;\n"
        "border-bottom: 1px solid #717072;\n"
        "}\n";
}


AuthenticationDialog::AuthenticationDialog(QWidget* parent) :
    QWidget(parent)
{
    resize(720, 720);

    setWindowTitle(PROJECT_NAME);

    setStyleSheet(
        "*{\n"
        "font-family: centry gothic;\n"
        "font-size: 24px;\n"
        "}"
    );

    _firstAuthStageFrame = new QFrame(this);

    _firstAuthStageFrame->setFrameShape(QFrame::Shape::StyledPanel);
    _firstAuthStageFrame->setFrameShadow(QFrame::Shadow::Raised);

    _confirmCredentialsButton = new QPushButton(this);
    _confirmAuthCodeButton = new QPushButton(this);
    _backToFirstFrameButton = new QPushButton(this);
    _sendCodeAgainButton = new QPushButton(this);

    _confirmCredentialsButton->setStyleSheet(buttonStyle);
    _confirmAuthCodeButton->setStyleSheet(buttonStyle);
    _backToFirstFrameButton->setStyleSheet(buttonStyle);
    _sendCodeAgainButton->setStyleSheet(buttonStyle);

    _confirmCredentialsButton->setText("Войти");
    _confirmAuthCodeButton->setText("Подтвердить");
    _backToFirstFrameButton->setText("Назад");
    _sendCodeAgainButton->setText("Отправить ещё раз");

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
        _firstAuthStageFrame->isVisible() ? toSecondFrame() : toFirstFrame();
        
        const auto apiHash = _apiHashLineEdit->text().toStdString();
        const auto apiId = _apiIdLineEdit->text().toInt();
        const auto phoneNumber = _phoneNumberLineEdit->text().toStdString();

        if (_skipFirstAuthenticationStage == false) {
            TelegramCredentials credentials;

            credentials.apiHash = apiHash;
            credentials.apiId = apiId;
            credentials.phoneNumber = phoneNumber.length() != 0 ?
                phoneNumber.at(0) == '+' ?
                phoneNumber : "+" + phoneNumber : "";

            emit credentialsAccepted(credentials);
        }
    });

    connect(_confirmAuthCodeButton, &QPushButton::clicked, [this]() {
        _firstAuthStageFrame->isVisible() ? toSecondFrame() : toFirstFrame();

        QString mobilePhoneCode = _telegramCodeLineEdit->text();

        if (mobilePhoneCode.length() < 5) {
            //_incorrectTelegramCodeLabel->show();
            _telegramCodeLineEdit->clear();
            shake();
            return;
        }
    });

    connect(_backToFirstFrameButton, &QPushButton::clicked, [this]() {
        toFirstFrame();
    });
        
    toFirstFrame();
}

void AuthenticationDialog::setCloseAbility(bool close) {
    _canClose = close;
}

void AuthenticationDialog::toSecondFrame() {
    hideWidgets();
    _firstAuthStageFrame->show();
    _confirmAuthCodeButton->show();
   // _backToFirstFrameButton->show();
    _telegramCodeLineEdit->show();
    _sendCodeAgainButton->show();
}

void AuthenticationDialog::toFirstFrame() {
    hideWidgets();
    _firstAuthStageFrame->show();
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
    updateFrameGeometry();
}

void AuthenticationDialog::closeEvent(QCloseEvent* event) {
    if (_canClose) {
        QWidget::closeEvent(event);
    }
    else {
        shake();
    }
}

void AuthenticationDialog::vacillate()
{
    QPoint offset(5, 10);
    move(((_shakeSwitch) ? pos() + offset : pos() - offset));

    _shakeSwitch = !_shakeSwitch;
}

void AuthenticationDialog::hideWidgets() {
    foreach(auto item, children()) {
        if (qobject_cast<QWidget*>(item)) {
            qobject_cast<QWidget*>(item)->hide();
        }
    }
}

void AuthenticationDialog::showWidgets() {
    foreach(auto item, children()) {
        if (qobject_cast<QWidget*>(item)) {
            qobject_cast<QWidget*>(item)->show();
        }
    }
}

void AuthenticationDialog::updateFrameGeometry() {
    const auto widgetsWidth = width() / 4.;
    const auto widgetsHeight = height() / 12.;

    const auto centerX = (width() - widgetsWidth) / 2.;

    const auto authFrameWidth = widgetsWidth * 1.2;
    const auto authFrameHeight = widgetsHeight * 5.;

    _confirmCredentialsButton->resize(widgetsWidth, widgetsHeight);
    _confirmAuthCodeButton->resize(widgetsWidth / 2., widgetsHeight);
    _backToFirstFrameButton->resize(widgetsWidth / 2., widgetsHeight);
    _sendCodeAgainButton->resize(widgetsWidth / 2., widgetsHeight);

    _apiHashLineEdit->resize(widgetsWidth, widgetsHeight);
    _apiIdLineEdit->resize(widgetsWidth, widgetsHeight);
    _phoneNumberLineEdit->resize(widgetsWidth, widgetsHeight);
    _telegramCodeLineEdit->resize(widgetsWidth, widgetsHeight);
    
    _confirmCredentialsButton->move(centerX, height() - widgetsHeight * 1.5);
 
    _sendCodeAgainButton->move(centerX - _sendCodeAgainButton->width(), height() - widgetsHeight * 1.5);
    _confirmAuthCodeButton->move(centerX + _confirmAuthCodeButton->width(), height() - widgetsHeight * 3.);

    _backToFirstFrameButton->move((width() - widgetsWidth / 2.) / 2., height() - widgetsHeight * 1.5);
    _apiHashLineEdit->move(centerX, height() - widgetsHeight * 3.5);

    _telegramCodeLineEdit->move(centerX, height() - widgetsHeight * 5.5);
    _apiIdLineEdit->move(centerX, height() - widgetsHeight * 4.5);

    _phoneNumberLineEdit->move(centerX, height() - widgetsHeight * 5.5);

    _firstAuthStageFrame->move((width() - authFrameWidth) / 2., (height() - authFrameHeight) / 2.);
    _firstAuthStageFrame->resize(authFrameWidth,
        authFrameHeight);
}