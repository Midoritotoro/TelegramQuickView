﻿#pragma once

#include <QSlider>
#include <QLabel>
#include <QStyle>
#include <QMouseEvent>

class EnhancedSlider : public QSlider
{
private:
    Q_OBJECT
    uint16_t m_HandleLen;
    bool bIsMove = false;
public:
    EnhancedSlider(Qt::Orientation orientation = Qt::Horizontal, uint16_t handleLen = 0, QWidget* parent = nullptr);

    void setSliderValue(int val, bool bNotify = false);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    [[nodiscard]] int32_t mousePostionToSliderVal(const QPoint& pos);
};