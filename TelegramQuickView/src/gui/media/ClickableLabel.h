#pragma once

#include <QLabel>

class ClickableLabel: public QLabel
{
private:
    Q_OBJECT
signals:
    void clicked();
public:
    void mousePressEvent(QMouseEvent* event);
    using QLabel::QLabel;
};