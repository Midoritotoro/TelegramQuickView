#include "EnhancedSlider.h"

static bool bIsMove = false;

EnhancedSlider::EnhancedSlider(Qt::Orientation orientation, uint16_t handleLen, QWidget* parent)
    : QSlider(parent), m_HandleLen(handleLen) 
{
    setOrientation(orientation);
    setAttribute(Qt::WA_NoSystemBackground);
    setCursor(Qt::PointingHandCursor);
}

EnhancedSlider::~EnhancedSlider() {

}

void EnhancedSlider::SetSliderValue(int val, bool bNotify) {
    setSliderPosition(val);
    if (bNotify)
        emit sliderMoved(val);
}

void EnhancedSlider::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton)
        return;
    setSliderDown(true);
    int32_t pos = MousePostionToSliderVal(event->pos());
        if (pos != sliderPosition()) {
            setSliderPosition(pos);
            emit sliderMoved(pos);
    }
}

void EnhancedSlider::mouseMoveEvent(QMouseEvent* event) {
    QSlider::mouseMoveEvent(event);
    bIsMove = true;
}

void EnhancedSlider::mouseReleaseEvent(QMouseEvent* event) {
    setSliderDown(false);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        bIsMove = false;
        return;
    }

    if (bIsMove) {
        QSlider::mouseReleaseEvent(event);
        bIsMove = false;
    }
    else {
        int32_t pos = MousePostionToSliderVal(event->pos());
        if (pos != sliderPosition()) {
            setSliderPosition(pos);
            emit sliderMoved(pos);
        }
    }
}

int32_t EnhancedSlider::MousePostionToSliderVal(const QPoint& pos) {
    int32_t duration = maximum() - minimum();
    int32_t sliderPos = 0;
    qreal mousePos = 0;
    uint16_t handleLen = 0;
    if (Qt::Vertical == orientation()) {
        handleLen = m_HandleLen ? m_HandleLen : width() / 2.0;
        mousePos = (qreal)(pos.y() - handleLen / 2);
        if (0 > mousePos)
            mousePos = 0;
        else if (mousePos > height() - handleLen)
            mousePos = (qreal)(height() - handleLen);
        sliderPos = maximum() - duration * (mousePos / (height() - handleLen));
    }
    else {
        handleLen = m_HandleLen ? m_HandleLen : height() / 2.0;
        mousePos = (qreal)(pos.x() - handleLen / 2);
        if (0 > mousePos)
            mousePos = 0;
        else if (pos.x() > width() - handleLen)
            mousePos = (qreal)(width() - handleLen);
        sliderPos = minimum() + duration * (mousePos / (width() - handleLen));
    }
    return sliderPos;
}