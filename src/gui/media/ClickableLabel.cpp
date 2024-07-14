#include "ClickableLabel.h"

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
}