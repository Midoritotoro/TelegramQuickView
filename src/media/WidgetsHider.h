#pragma once

#include <atomic>

#include <QApplication>
#include <QTimer>
#include <QWidget>
#include <QWidgetList>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>


class WidgetsHider : public QObject
{
    Q_OBJECT
public:
    using DurationT = int;

    explicit inline WidgetsHider(QWidgetList& qWidgetList, bool fadeInOutAnimation)
        : _inactivityDuration{ DurationT{4000} }
        , _qWidgetList(qWidgetList)
        , _fadeInOutAnimation(fadeInOutAnimation)
        , _animationDuration(DurationT{ 2000 })
    {
        QCoreApplication::instance()->installEventFilter(this);
        _timer.setSingleShot(true);
        _timer.callOnTimeout(
            [this]() {
                ControlVisibility(false);
            }
        );
        if (fadeInOutAnimation) {
            _animationTimer.setSingleShot(true);
            _animationTimer.callOnTimeout(
                [this]() {
                    ControlAnimationVisibility(false);
                });
        }
    }

    inline void SetInactivityDuration(DurationT msecs) {
        _inactivityDuration = msecs;
    }

    inline void SetAnimationDuration(DurationT msecs) {
        _animationDuration = msecs;
    }

Q_SIGNALS:
    void widgetsHidden();
    void widgetsShowed();
private:
    std::atomic<DurationT> _inactivityDuration, _animationDuration;
    QTimer _timer, _animationTimer;
    QWidgetList _qWidgetList;
    bool _fadeInOutAnimation;

    inline bool eventFilter(QObject* pWatched, QEvent* pEvent) override {
        if (pEvent->type() == QEvent::MouseMove
            || pEvent->type() == QEvent::MouseButtonPress) {

            ControlVisibility(true);

            if (_inactivityDuration != DurationT{})
                _timer.start(_inactivityDuration);

            if (_fadeInOutAnimation) {
                ControlAnimationVisibility(true);
                if (_inactivityDuration != DurationT{})
                    _animationTimer.start(_animationDuration);
            }
        }

        return QObject::eventFilter(pWatched, pEvent);
    }

    void FadeInAnimation(QWidget* widget) { // Появление
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
        widget->setGraphicsEffect(effect);
        QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(effect, "opacity");
        fadeInAnimation->setDuration(_animationDuration);
        fadeInAnimation->setStartValue(0);
        fadeInAnimation->setEndValue(1);
        fadeInAnimation->setEasingCurve(QEasingCurve::InBack);
        fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    }

    void FadeOutAnimation(QWidget* widget) { // Исчезание
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
        widget->setGraphicsEffect(effect);
        QPropertyAnimation* fadeOutAnimation = new QPropertyAnimation(effect, "opacity");
        fadeOutAnimation->setDuration(_animationDuration);
        fadeOutAnimation->setStartValue(1);
        fadeOutAnimation->setEndValue(0);
        fadeOutAnimation->setEasingCurve(QEasingCurve::InBack);
        fadeOutAnimation->start(QPropertyAnimation::DeleteWhenStopped);

    }

    inline void ControlAnimationVisibility(bool Show) {
        foreach(QWidget * widget, _qWidgetList)
            widget->setVisible(Show);
        Show ? emit widgetsShowed() : emit widgetsHidden();
    }

    inline void ControlVisibility(bool Show) {
        foreach(QWidget * widget, _qWidgetList) {
            if (_fadeInOutAnimation)
                Show ? FadeOutAnimation(widget) : FadeInAnimation(widget);
            else
                widget->setVisible(Show);
        }
        if (!_fadeInOutAnimation)
            Show ? emit widgetsShowed() : emit widgetsHidden();
    }
};