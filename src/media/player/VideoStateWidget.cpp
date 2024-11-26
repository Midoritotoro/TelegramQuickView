#include "VideoStateWidget.h"

#include "../../core/StyleCore.h"

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
    update();
}

VideoStateWidget::State VideoStateWidget::state() const noexcept {
    return _state;
}

void VideoStateWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);

    if (_currentPixmap.size() != size()) {
        auto image = QImage();

        switch (_state) {
            case State::Play:
                image = QImage(_playImagePath);
                break;

            case State::Pause:
                image = QImage(_pauseImagePath);
                break;

            case State::Repeat:
                image = QImage(_repeatImagePath);
                break;
        }

        image = style::Prepare(image, size());
        image = std::move(image).scaled(
            image.width() * style::DevicePixelRatio(),
            image.height() * style::DevicePixelRatio(),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation);

        _currentPixmap = QPixmap::fromImage(std::move(image), Qt::ColorOnly);
        _currentPixmap.setDevicePixelRatio(style::DevicePixelRatio());
    }

    painter.drawPixmap(0, 0, _currentPixmap);
}

void VideoStateWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}