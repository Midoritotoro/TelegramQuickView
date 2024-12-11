#pragma once

#include <QPushButton>

class VideoStateWidget : public QPushButton {
    Q_OBJECT
public:
    enum class State {
        Play,
        Pause,
        Repeat
    };

    VideoStateWidget(QWidget* parent = nullptr);

    void setState(State state);
    [[nodiscard]] State state() const noexcept;
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QPixmap _pausePixmap, _playPixmap, _repeatPixmap;

    State _state;
};