#include "EnhancedSlider.h"

#include <QPainter>
#include <QStyleOptionSlider>
#include <QStylePainter>


EnhancedSlider::EnhancedSlider(QWidget* parent, Qt::Orientation orientation, uint16_t handleLen)
    : QSlider(parent), m_HandleLen(handleLen) 
{
    setContentsMargins(0, 0, 0, 0);
    setOrientation(orientation);
    setCursor(Qt::PointingHandCursor);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void EnhancedSlider::setSliderValue(int val, bool bNotify) {
    setSliderPosition(val);
    if (bNotify)
        emit sliderMoved(val);
}

void EnhancedSlider::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton)
        return;
    setSliderDown(true);
    int32_t pos = mousePostionToSliderVal(event->pos());
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
        int32_t pos = mousePostionToSliderVal(event->pos());
        if (pos != sliderPosition()) {
            setSliderPosition(pos);
            emit sliderMoved(pos);
        }
    }
}

void EnhancedSlider::paintEvent(QPaintEvent* event)  {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //QStyleOptionSlider opt;
    //opt.initFrom(this);

    //QStyle* styl = style();
    //opt.state &= ~QStyle::State_HasFocus;

    //QRect rectGroove = styl->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    //QRect rectHandle = styl->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    //opt.rect = rect();
    //if (isEnabled())
    //    opt.state |= QStyle::State_Enabled;
    //opt.orientation = (orientation() == Qt::Vertical) ? Qt::Vertical : Qt::Horizontal;

    //if (orientation() == Qt::Horizontal)
    //    opt.state |= QStyle::State_Horizontal;
    //else
    //    opt.state &= ~QStyle::State_Horizontal;

    //opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
    //opt.sliderValue = value();
    //opt.sliderPosition = opt.sliderValue;
    //opt.pageStep = pageStep();
    //opt.minimum = 0;
    //opt.maximum = qMax(0, maximum());

   // opt.palette.setBrush(QPalette::ColorGroup::All, QPalette::ColorRole::Base, QColor(Qt::black));

   // QSlider::paintEvent(event);


    //setAttribute(Qt::WA_OpaquePaintEvent, true);
    painter.fillRect(rect(), Qt::black);

  //  styl->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);
    QSlider::paintEvent(event);

}

int32_t EnhancedSlider::mousePostionToSliderVal(const QPoint& pos) {
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