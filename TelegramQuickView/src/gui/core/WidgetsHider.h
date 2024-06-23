#pragma once

#include <atomic>

#include <QApplication>
#include <QTimer>
#include <QSlider>

class WidgetsHider : public QObject
{
    Q_OBJECT

public:
    using DurationT = int;

    static inline WidgetsHider& Instance(QWidgetList& qWidgetList)
    {
        static WidgetsHider i(qWidgetList);
        return i;
    }

    explicit inline WidgetsHider(QWidgetList& qWidgetList) : _InactivityDuration{ DurationT{} }, _qWidgetList(qWidgetList)
    {
        QCoreApplication::instance()->installEventFilter(this);

        _Timer.setSingleShot(true);
        _Timer.callOnTimeout(
            [this]() {
                ControlVisibility(false);
            }
        );
    }

    inline void SetInactivityDuration(DurationT Value)
    {
        _InactivityDuration = Value;
    }
Q_SIGNALS:
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_GCC("-Wenum-compare")
    void widgetsHidden();
    void widgetsShowed();
    QT_WARNING_POP
private:
    std::atomic<DurationT> _InactivityDuration;
    QTimer _Timer;
    QWidgetList _qWidgetList;

    inline bool eventFilter(QObject* pWatched, QEvent* pEvent) override
    {
        if (pEvent->type() == QEvent::MouseMove)
        {
            ControlVisibility(true);

            if (_InactivityDuration != DurationT{}) {
                _Timer.start(_InactivityDuration);
            }
        }

        return QObject::eventFilter(pWatched, pEvent);
    }

    inline void ControlVisibility(bool Show)
    {
        foreach (QWidget* widget, _qWidgetList) {
            Show ? widget->show() : widget->hide();
        }
        Show ? emit widgetsShowed() : emit widgetsHidden();
    }
};