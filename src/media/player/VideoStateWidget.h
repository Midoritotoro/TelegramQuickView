#pragma once

#include "../ClickableLabel.h"
#include <QStackedWidget>

class VideoStateWidget : public QWidget {
private:
    Q_OBJECT
    Q_PROPERTY(QStackedWidget* _stackedWidget READ stackedWidget)
    ClickableLabel* _playClickableLabel = nullptr, * _stopClickableLabel = nullptr, *_repeatClickableLabel = nullptr;
    QStackedWidget* _stackedWidget = nullptr;
public:
    VideoStateWidget(ClickableLabel* playClickableLabel, ClickableLabel* stopClickableLabel, ClickableLabel* repeatClickableLabel, QWidget* parent = nullptr);
    [[nodiscard]] QStackedWidget* stackedWidget() { return _stackedWidget; }
};