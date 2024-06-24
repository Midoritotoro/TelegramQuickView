#include "ScrollBar.h"

#include <QPainter>
#include <QPen>

ScrollBar::ScrollBar(QWidget* parent) :
	QWidget(parent)
{
	CreateScrollBar();
}

ScrollBar::~ScrollBar()
{
}
void ScrollBar::CreateScrollBar()
{
    ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int PanelWidth = ScreenWidth / 3;
    int ChannelIconsScrollAreaWidth = PanelWidth / 3;
    setFixedSize(PanelWidth, ScreenHeight);
    setGeometry(ScreenWidth - width(), 0, width(), ScreenHeight);

    setContentsMargins(0, 0, 0, 0);
    GridLayout = new QGridLayout(this);

    GridLayout->setContentsMargins(0, 0, 0, 0);
    GridLayout->setSpacing(0);

    mainWidget = new QWidget();
    mainLayout = new QGridLayout(mainWidget);
    vMainWidget = new QWidget();
    IconScrollArea = new QScrollArea();
    vLayout = new QVBoxLayout(vMainWidget);

    IconScrollArea->setFixedSize(ChannelIconsScrollAreaWidth, ScreenHeight);

    IconScrollArea->setWidget(vMainWidget);
    IconScrollArea->setContentsMargins(0, 0, 0, 0);
    IconScrollArea->setWidgetResizable(true);
    IconScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    IconScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    IconScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    mainWidget->setContentsMargins(0, 0, 0, 0);

    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(GridLayout);

    for (int index = 0; index < 5; index++) {
        CreateTelegramChannelWidget("War Thunder" + QString::number(index), "C:\\Users\\danya\\Downloads\\image.jpg", mainWidget);
    }

    vLayout->addStretch();
    GridLayout->addWidget(IconScrollArea, 0, 0, 1, 1, Qt::AlignLeft);
    GridLayout->addWidget(mainWidget, 0, 1, 1, 1, Qt::AlignLeft);
}

void ScrollBar::IconChannelLabel_clicked()
{
    for (int i = 0; i < mainLayout->count(); i++) {
        QLayoutItem* pItem = mainLayout->itemAt(i);
        if (pItem->widget()) {
            pItem->widget()->hide();
        }
    }

    IconChannelLabel* IconLabel = (IconChannelLabel*)sender();
    IconLabel->setScrollAreaVisible(true);
}

void ScrollBar::CreateTelegramChannelWidget(const QString channelName, const QString imagePath, QWidget* parent)
{
    QWidget* widget = new QWidget();
    widget->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* hBoxLayout = new QHBoxLayout(widget);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);
    IconChannelLabel* _IconChannelLabel = new IconChannelLabel(channelName, imagePath, parent);
    _IconChannelLabel->setFixedSize(ScreenWidth / 35, ScreenHeight / 20);
    QRegion region(QRect(0, 0, _IconChannelLabel->width() - _IconChannelLabel->width() * 0.01, _IconChannelLabel->height() - _IconChannelLabel->height() * 0.01), QRegion::Ellipse);
    _IconChannelLabel->setMask(region);
    QLabel* TextLabel = new QLabel(_IconChannelLabel->getText());

    hBoxLayout->addWidget(_IconChannelLabel);
    hBoxLayout->addWidget(TextLabel);

    vLayout->addWidget(widget);

    mainLayout->addWidget(_IconChannelLabel->getLinkedScrollArea(), 0, 1, 1, 4, Qt::AlignLeft);
    mainLayout->addWidget(_IconChannelLabel->getLinkedLabel(), 0, 1, 1, 1);

    _IconChannelLabel->setScrollAreaVisible(false);
    connect(_IconChannelLabel, SIGNAL(clicked()), this, SLOT(IconChannelLabel_clicked()));
}