#include "VideoStateWidget.h"


VideoStateWidget::VideoStateWidget(ClickableLabel* playClickableLabel, ClickableLabel* stopClickableLabel, ClickableLabel* repeatClickableLabel, QWidget* parent):
    QWidget(parent), _playClickableLabel(playClickableLabel), _stopClickableLabel(stopClickableLabel), _repeatClickableLabel(repeatClickableLabel)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setCursor(Qt::PointingHandCursor);
    _stackedWidget = new QStackedWidget(this);
    _stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _stackedWidget->addWidget(_stopClickableLabel);
    _stackedWidget->addWidget(_playClickableLabel);
    _stackedWidget->addWidget(_repeatClickableLabel);

    _stackedWidget->setCurrentIndex(0);
 }
