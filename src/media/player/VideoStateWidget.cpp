#include "VideoStateWidget.h"

#include <QPainter>
#include <QCoreApplication>
#include <QDir>


VideoStateWidget::VideoStateWidget(QWidget* parent):
    QPushButton(parent)
{
    QString currentPath = QCoreApplication::applicationDirPath();
    QDir assetsDir(currentPath + "/../../assets/images");

    _pauseImagePath = assetsDir.absolutePath() + "/stop.png";
    _playImagePath = assetsDir.absolutePath() + "/play.png";
    _repeatImagePath = assetsDir.absolutePath() + "/repeat.png";

    setAttribute(Qt::WA_NoSystemBackground);
    setCursor(Qt::PointingHandCursor);
    setState(State::Pause);
}

void VideoStateWidget::setState(State state) {
    if (_state == state)
        return;

    _state = state;
    repaint();
}

VideoStateWidget::State VideoStateWidget::state() const noexcept {
    return _state;
}

void VideoStateWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    switch (_state) {
        case State::Play:
            _currentPixmap.load(_playImagePath);
            break;

        case State::Pause:
            _currentPixmap.load(_pauseImagePath);
            break;

        case State::Repeat:
            _currentPixmap.load(_repeatImagePath);
            break;
    }

    if (_currentPixmap.size() != size())
        _currentPixmap = _currentPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPixmap(0, 0, _currentPixmap);
}

void VideoStateWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}