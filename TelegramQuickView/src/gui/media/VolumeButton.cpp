#include "VolumeButton.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

#include <Windows.h>


VolumeButton::VolumeButton(const QImage& image, QSlider* VolumeSlider, QWidget* parent) :
    ImagePushButton(image, parent), m_VolumeSlider(VolumeSlider)
{
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    setAttribute(Qt::WA_NoSystemBackground);
    setCursor(Qt::PointingHandCursor);
    setFixedSize(screenWidth / 60, screenHeight / 40);
}

void VolumeButton::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);
    m_VolumeSlider->hide();
}
