#pragma once

#include "../ClickableLabel.h"
#include <QPushButton>


class VideoStateWidget : public QPushButton {
    Q_OBJECT
public:
    enum class State {
        Play,
        Pause,
        Repeat
    };
private:
    QString _pauseImagePath, _playImagePath, _repeatImagePath;
    State _state;
public:
    VideoStateWidget(QWidget* parent = nullptr);

    void setState(State state);
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};