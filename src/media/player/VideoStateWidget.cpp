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
    QPushButton::paintEvent(event);

    if (_pauseImagePath.isEmpty() || _playImagePath.isEmpty() || _repeatImagePath.isEmpty())
        return;

    QPixmap pixmap;

    switch (_state) {
        case State::Play:
            pixmap = QPixmap(_playImagePath);
            break;
        case State::Pause:
            pixmap = QPixmap(_pauseImagePath);
            break;
        case State::Repeat:
            pixmap = QPixmap(_repeatImagePath);
            break;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (pixmap.size() != size())
        pixmap = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPixmap(0, 0, pixmap);
}

void VideoStateWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

}