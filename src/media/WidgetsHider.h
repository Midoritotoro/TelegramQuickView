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

    static inline WidgetsHider& Instance(QWidgetList& qWidgetList, bool fadeInOutAnimation = false) {
        static WidgetsHider i(qWidgetList, fadeInOutAnimation);
        return i;
    }

    explicit inline WidgetsHider(QWidgetList& qWidgetList, bool fadeInOutAnimation) : _inactivityDuration{ DurationT{} },
        _qWidgetList(qWidgetList), _fadeInOutAnimation(fadeInOutAnimation) 
    {
        QCoreApplication::instance()->installEventFilter(this);

        _timer.setSingleShot(true);
        _timer.callOnTimeout(
            [this]() {
                ControlVisibility(false);
            }
        );
    }

    inline void SetInactivityDuration(DurationT Value) {
        _inactivityDuration = Value;
    }
Q_SIGNALS:
    void widgetsHidden();
    void widgetsShowed();
private:
    std::atomic<DurationT> _inactivityDuration;
    QTimer _timer;
    QWidgetList _qWidgetList;
    bool _fadeInOutAnimation;

    inline bool eventFilter(QObject* pWatched, QEvent* pEvent) override {
        if (pEvent->type() == QEvent::MouseMove) {
            
            ControlVisibility(true);

            if (_inactivityDuration != DurationT{})
                _timer.start(_inactivityDuration);
        }

        return QObject::eventFilter(pWatched, pEvent);
    }

    void FadeInAnimation(QWidget* widget) {

        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this); 
        widget->setGraphicsEffect(effect);
        QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(effect, "opacity");
        fadeInAnimation->setDuration(_inactivityDuration);
        fadeInAnimation->setStartValue(0);
        fadeInAnimation->setEndValue(1);
        fadeInAnimation->setEasingCurve(QEasingCurve::InBack);
        fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    }

    void FadeOutAnimation(QWidget* widget) {
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
        widget->setGraphicsEffect(effect);
        QPropertyAnimation* fadeOutAnimation = new QPropertyAnimation(effect, "opacity");
        fadeOutAnimation->setDuration(_inactivityDuration);
        fadeOutAnimation->setStartValue(1);
        fadeOutAnimation->setEndValue(0);
        fadeOutAnimation->setEasingCurve(QEasingCurve::InBack);
        fadeOutAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    }

    inline void ControlVisibility(bool Show) {
        foreach(QWidget * widget, _qWidgetList)
        {
            if (_fadeInOutAnimation)
                Show ? FadeOutAnimation(widget) : FadeInAnimation(widget);
            else {
                widget->setVisible(Show);
                qDebug() << !widget->isHidden();
            }
        }
       Show ? emit widgetsShowed() : emit widgetsHidden();
    }
};