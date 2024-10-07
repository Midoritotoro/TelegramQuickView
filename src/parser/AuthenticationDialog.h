#pragma once



#include <QDialog>

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include <QCloseEvent>

#include "UserDataManager.h"
#include "OpenGLBlur.h"


class AuthenticationDialog: public QWidget
{
    Q_OBJECT
private:
    bool _shakeSwitch = true, _skipFirstAuthenticationStage = false, _canClose = false;
    QPushButton* _confirmCredentialsButton = nullptr, * _confirmAuthCodeButton = nullptr, * _backToFirstFrameButton = nullptr, *_sendCodeAgainButton = nullptr;
    QLineEdit* _apiHashLineEdit = nullptr, *_apiIdLineEdit = nullptr, *_phoneNumberLineEdit = nullptr, *_telegramCodeLineEdit = nullptr;
    QFrame* _blurredFrame = nullptr;
    QPixmap _background;
    QString _pathToBackgroundImage;
    GLBlurFunctions _GLBlurDualKawase;
public:
    AuthenticationDialog(QWidget* parent = nullptr);

    void shake();
    void setCloseAbility(bool close);
    void toSecondFrame();
    void toFirstFrame();

    [[nodiscard]] QSize getMinimumSizeWithAspectRatio(const QSize& imageSize, const int parentWidth) {
        return QSize(parentWidth, parentWidth * imageSize.height() / imageSize.width());
    }
Q_SIGNALS:
    void credentialsAccepted(const TelegramCredentials& credentials);
    void authCodeAccepted(const QString code);
private:
    void vacillate();
    void hideWidgets();
    void showWidgets();
    void updateFrameGeometry();
protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
};