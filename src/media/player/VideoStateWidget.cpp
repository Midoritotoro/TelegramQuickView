#include "VideoStateWidget.h"

#include "../../core/StyleCore.h"

#include <QPainter>
#include <QCoreApplication>
#include <QDir>


VideoStateWidget::VideoStateWidget(QWidget* parent):
    QPushButton(parent)
{
    const auto currentPath = QCoreApplication::applicationDirPath();
    const auto assetsDir = QDir(currentPath + "/../../assets/images");

    _pausePixmap = QPixmap(assetsDir.absolutePath() + "/stop.png");
    _playPixmap = QPixmap(assetsDir.absolutePath() + "/play.png");
    _repeatPixmap = QPixmap(assetsDir.absolutePath() + "/repeat.png");

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
    auto painter = QPainter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::NoBrush);

    auto& image = QImage();

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

    image.setDevicePixelRatio(style::DevicePixelRatio());
    painter.drawPixmap(0, 0, image);
}