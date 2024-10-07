#include "AuthenticationDialog.h"

#include <QTimer>
#include <QFontMetrics>
#include <QApplication>
#include <QScreen>
#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <QGraphicsBlurEffect>


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
            "color: black;\n"
        "}\n"
        "QPushButton::hover { \n"
            "background: NavajoWhite;\n"
        "}";

    const QString backButtonStyle = "QPushButton{\n"
            "background: rgb(128, 128, 128);\n"
            "color: black;\n"
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
}


AuthenticationDialog::AuthenticationDialog(QWidget* parent) :
    QWidget(parent)
{
    setWindowTitle(PROJECT_NAME);

    _blurredFrame = new QFrame(this);

    _blurredFrame->setFrameShape(QFrame::Shape::StyledPanel);
    _blurredFrame->setFrameShadow(QFrame::Shadow::Raised);
    _blurredFrame->setAttribute(Qt::WA_NoSystemBackground);
    _blurredFrame->setAttribute(Qt::WA_TranslucentBackground);
    _blurredFrame->setAttribute(Qt::WA_TransparentForMouseEvents);
    _blurredFrame->setStyleSheet("background: rgba(0, 0, 0, 90)");
        
    QString currentPath = QCoreApplication::applicationDirPath();
    QDir assetsDir(currentPath + "/../../assets/images");

    QPalette _palette = _blurredFrame->palette();
    _palette.setBrush(QPalette::ColorRole::Base, QBrush(QImage(assetsDir.absolutePath() + "/frameBackground.png")));
    _blurredFrame->setPalette(_palette);

    _pathToBackgroundImage = assetsDir.absolutePath() + "/main_screen.jpg";
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
        toSecondFrame();
        
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
    _blurredFrame->show();
    _confirmAuthCodeButton->show();
    _backToFirstFrameButton->show();
    _telegramCodeLineEdit->show();
    _sendCodeAgainButton->show();
}

void AuthenticationDialog::toFirstFrame() {
    hideWidgets();
    _blurredFrame->show();
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

    _background = _background.scaled(getMinimumSizeWithAspectRatio(_background.size(), _background.size().width() / 4.), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setFixedSize(_background.size());

    updateFrameGeometry();
}

void AuthenticationDialog::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    if (_background.isNull())
        return;


    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, _background);

    QPalette _palette = _blurredFrame->palette();
    const QBrush& brush = _palette.brush(QPalette::ColorRole::Base);

    QImage blurredImage = _GLBlurDualKawase.blurImage_DualKawase(brush.texture().scaled(_blurredFrame->size(),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage(), 1, 2);

    QBrush blurredBrush = brush;
    blurredBrush.setTextureImage(blurredImage);
    QColor blurredBrushColor = blurredBrush.color();
    blurredBrushColor.setAlpha(0);
    blurredBrush.setColor(blurredBrushColor);

    _palette.setBrush(QPalette::ColorRole::Base, blurredBrush);

    _blurredFrame->setPalette(_palette);
   // painter.drawPixmap(_blurredFrame->rect(), QPixmap::fromImage(blurredImage));
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
    const auto widgetsHeight = height() / 14.;

    const auto centerX = (width() - widgetsWidth) / 2.;

    const auto authFrameWidth = widgetsWidth * 1.2;
    const auto authFrameHeight = widgetsHeight * 5.;

    _confirmCredentialsButton->resize(widgetsWidth, widgetsHeight);
    _confirmAuthCodeButton->resize(widgetsWidth, widgetsHeight);
    _backToFirstFrameButton->resize(widgetsWidth / 3., widgetsHeight / 1.5);
    _sendCodeAgainButton->resize(widgetsWidth / 3., widgetsHeight / 1.5);

    _apiHashLineEdit->resize(widgetsWidth, widgetsHeight);
    _apiIdLineEdit->resize(widgetsWidth, widgetsHeight);
    _phoneNumberLineEdit->resize(widgetsWidth, widgetsHeight);
    _telegramCodeLineEdit->resize(widgetsWidth, widgetsHeight);
    
    _confirmCredentialsButton->move(centerX, (height() + widgetsHeight * 3.) / 2.);
    _confirmAuthCodeButton->move(centerX, (height() - widgetsHeight / 1.5) / 2.);
 
    _phoneNumberLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2. - widgetsHeight * 1.5);
    _apiIdLineEdit->move(centerX, (height() - widgetsHeight) / 2. - widgetsHeight * 0.5);
    _apiHashLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2.);

    _sendCodeAgainButton->move((centerX), (height() + widgetsHeight * 1.5) / 2.);
    _backToFirstFrameButton->move((centerX + widgetsWidth / 1.5), (height() + widgetsHeight * 1.5) / 2.);

    _telegramCodeLineEdit->move(centerX, (height() - widgetsHeight * 0.5) / 2. - widgetsHeight * 1.5);

    _blurredFrame->resize(authFrameWidth, authFrameHeight);
    _blurredFrame->move((width() - authFrameWidth) / 2., (height() - authFrameHeight) / 2.);
}