﻿#pragma once

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

    explicit inline WidgetsHider(QWidgetList& qWidgetList, bool fadeInOutAnimation) : _inactivityDuration{ DurationT{} },
        _qWidgetList(qWidgetList), _fadeInOutAnimation(fadeInOutAnimation) 
    {
        QCoreApplication::instance()->installEventFilter(this);
        _effect = new QGraphicsOpacityEffect(this);
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
    QGraphicsOpacityEffect* _effect = nullptr;

    inline bool eventFilter(QObject* pWatched, QEvent* pEvent) override {
        if (pEvent->type() == QEvent::MouseMove) {
            
            ControlVisibility(true);

            if (_inactivityDuration != DurationT{})
                _timer.start(_inactivityDuration);
        }

        return QObject::eventFilter(pWatched, pEvent);
    }

    void FadeInAnimation(QWidget* widget) { // Появление
        widget->setGraphicsEffect(_effect);
        QPropertyAnimation* fadeInAnimation = new QPropertyAnimation(_effect, "opacity");
        fadeInAnimation->setDuration(1200);
        fadeInAnimation->setStartValue(0);
        fadeInAnimation->setEndValue(1);
        fadeInAnimation->setEasingCurve(QEasingCurve::InBack);
        fadeInAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    }

    void FadeOutAnimation(QWidget* widget) { // Исчезание
        widget->setGraphicsEffect(_effect);
        QPropertyAnimation* fadeOutAnimation = new QPropertyAnimation(_effect, "opacity");
        fadeOutAnimation->setDuration(1200);
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
            else
                widget->setVisible(Show);
        }
       Show ? emit widgetsShowed() : emit widgetsHidden();
    }
};