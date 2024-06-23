#pragma once

#include <QSlider>
#include <QLabel>
#include <QStyle>
#include <QMouseEvent>

class EnhancedSlider : public QSlider
{
private:
    Q_OBJECT
    uint16_t m_HandleLen;
public:
    EnhancedSlider(Qt::Orientation orientation = Qt::Horizontal, uint16_t handleLen = 0, QWidget* parent = nullptr);
    ~EnhancedSlider();
    void SetSliderValue(int val, bool bNotify = false);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    int32_t MousePostionToSliderVal(const QPoint& pos);
};