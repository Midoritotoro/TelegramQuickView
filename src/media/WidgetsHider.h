#pragma once

#include <atomic>

#include <QApplication>
#include <QTimer>
#include <QWidget>
#include <QWidgetList>


class WidgetsHider : public QObject
{
    Q_OBJECT
public:
    using DurationT = int;

    static inline WidgetsHider& Instance(QWidgetList& qWidgetList) {
        static WidgetsHider i(qWidgetList);
        return i;
    }

    explicit inline WidgetsHider(QWidgetList& qWidgetList) : _inactivityDuration{ DurationT{} }, _qWidgetList(qWidgetList) {
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

    inline bool eventFilter(QObject* pWatched, QEvent* pEvent) override {
        if (pEvent->type() == QEvent::MouseMove) {
            ControlVisibility(true);

            if (_inactivityDuration != DurationT{})
                _timer.start(_inactivityDuration);
        }

        return QObject::eventFilter(pWatched, pEvent);
    }

    inline void ControlVisibility(bool Show) {
        foreach (QWidget* widget, _qWidgetList)
            widget->setVisible(Show);

        Show ? emit widgetsShowed() : emit widgetsHidden();
    }
};