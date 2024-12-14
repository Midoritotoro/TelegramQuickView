#include "VideoStateWidget.h"

#include "../../../ui/style/StyleCore.h"

#include <QPainter>
#include <QCoreApplication>
#include <QDir>


VideoStateWidget::VideoStateWidget(QWidget* parent):
    QPushButton(parent)
{
    const auto currentPath = QCoreApplication::applicationDirPath();
    const auto assetsDir = QDir(currentPath + "/../../assets/images");

    _pause = QImage(assetsDir.absolutePath() + "/stop.png");
    _play = QImage(assetsDir.absolutePath() + "/play.png");
    _repeat = QImage(assetsDir.absolutePath() + "/repeat.png");

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

    auto image = QImage();

    switch (_state) {
        case State::Play:
            image = _play;
            break;

        case State::Pause:
            image = _pause;
            break;

        case State::Repeat:
            image = _repeat;
            break;
    }

    image = style::Prepare(image, size());
    image = std::move(image).scaled(
        image.width() * style::DevicePixelRatio(),
        image.height() * style::DevicePixelRatio(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);

    image.setDevicePixelRatio(style::DevicePixelRatio());
    painter.drawImage(0, 0, image);
}