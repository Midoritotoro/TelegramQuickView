#pragma once



#include <QDialog>

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>

#include "data/UserDataManager.h"
#include "utils.h"



class AuthenticationDialog: public QDialog
{
    Q_OBJECT
private:
    Telegram::ErrorCodes _currentErrorCode;
    int _sleepSeconds = 0;
    bool _shakeSwitch = true, _skipFirstAuthenticationStage = false, _canClose = false;
    QPushButton* _confirmCredentialsButton = nullptr, * _confirmAuthCodeButton = nullptr, * _backToFirstFrameButton = nullptr, *_sendCodeAgainButton = nullptr;
    QLineEdit* _apiHashLineEdit = nullptr, *_apiIdLineEdit = nullptr, *_phoneNumberLineEdit = nullptr, *_telegramCodeLineEdit = nullptr;
    QPixmap _background;
    QString _pathToBackgroundImage;
public:
    AuthenticationDialog(QWidget* parent = nullptr);

    void shake();

    void setSleepSeconds(int seconds);
    void setErrorCode(Telegram::ErrorCodes code);
    void setCloseAbility(bool closeAbility);

    void toSecondFrame();
    void toFirstFrame();

    [[nodiscard]] QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
        return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
    }
Q_SIGNALS:
    void credentialsAccepted(const TelegramCredentials& credentials);
    void authCodeAccepted(const QString& code);
    void needSendCodeAgain();
private:
    void drawRoundedCorners(QPainter& painter, QRect rect, int borderRadius);

    void vacillate();
    void hideWidgets();

    void updateWidgetsGeometry();

    void paintErrorMessage(QPainter& painter);
protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
};