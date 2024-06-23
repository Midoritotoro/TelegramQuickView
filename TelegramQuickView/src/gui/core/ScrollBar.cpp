#include "ScrollBar.h"

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
    GridLayout = new QGridLayout(this);
    GridLayout->setContentsMargins(0, 0, 0, 0);
    int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int PanelWidth = ScreenWidth / 3;
    int ChannelIconsScrollAreaWidth = PanelWidth / 3;
    setFixedSize(PanelWidth, ScreenHeight);

    mainWidget = new QWidget();
    mainLayout = new QGridLayout(mainWidget);
    vMainWidget = new QWidget();
    IconScrollArea = new DynamicScrollArea(this);
    vLayout = new QVBoxLayout(vMainWidget);

    IconScrollArea->setWidget(vMainWidget);
    IconScrollArea->setFixedWidth(ChannelIconsScrollAreaWidth);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setContentsMargins(0, 0, 0, 0);
    GridLayout->setSpacing(0);
    setLayout(GridLayout);

    for (int index = 0; index < 5; index++) {
        CreateTelegramChannelWidget("War Thunder" + QString::number(index), QImage("C:\\Users\\danya\\Downloads\\avatar5815596965 (1).jpg"), mainWidget);
    }

    vLayout->addStretch();
    GridLayout->addWidget(IconScrollArea, 0, 0, 1, 1, Qt::AlignLeft);
    GridLayout->addWidget(mainWidget, 0, 1, 1, 1, Qt::AlignLeft);

    setGeometry(ScreenWidth - width(), 0, width(), ScreenHeight);
}

void ScrollBar::DynamicIconButton_clicked()
{
    for (int i = 0; i < mainLayout->count(); i++) {
        QLayoutItem* pItem = mainLayout->itemAt(i);
        if (pItem->widget()) {
            pItem->widget()->hide();
        }
    }

    DynamicIconChannelButton* button = (DynamicIconChannelButton*)sender();
    button->setScrollAreaVisible(true);
}

void ScrollBar::CreateTelegramChannelWidget(const QString channelName, const QImage& image, QWidget* parent)
{
    QWidget* widget = new QWidget();
    widget->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout * hBoxLayout = new QHBoxLayout(widget);
    DynamicIconChannelButton* dynamicButton = new DynamicIconChannelButton(channelName, image, parent, true);
    QLabel* TextLabel = new QLabel(dynamicButton->getText());

    dynamicButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hBoxLayout->addWidget(dynamicButton);
    hBoxLayout->addWidget(TextLabel);
    vLayout->addWidget(widget);
    mainLayout->addWidget(dynamicButton->getLinkedScrollArea(), 0, 1, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(dynamicButton->getLinkedLabel(), 0, 1, 1, 1);
    dynamicButton->setScrollAreaVisible(false);
    connect(dynamicButton, SIGNAL(clicked()), this, SLOT(DynamicIconButton_clicked()));
}