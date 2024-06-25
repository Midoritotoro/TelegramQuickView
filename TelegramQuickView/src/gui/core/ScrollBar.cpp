#include "ScrollBar.h"

#include "../media/ClickableLabel.h"

ScrollBar::ScrollBar(QWidget* parent) :
	QMainWindow(parent)
{
	CreateScrollBar();
}

void ScrollBar::CreateScrollBar() 
{
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    setGeometry(screenWidth - width(), 0, width(), screenHeight);

    QString currentPath = QCoreApplication::applicationDirPath();
    QDir assetsDir(currentPath + "/../../TelegramQuickView/assets/images");
    QString chatBackgroundPath = assetsDir.absolutePath() + "/chat_background.jpg";
    QString imagePath = "C:\\Users\\danya\\Downloads\\avatar.jpg";

    chatListWidget = new QWidget(this);

    setCentralWidget(chatListWidget);

    chatGridLayout = new QGridLayout(chatListWidget);
    chatList = new QListWidget(chatListWidget);

    chatListWidget->setContentsMargins(0, 0, 0, 0);
    chatGridLayout->setContentsMargins(0, 0, 0, 0);
    chatGridLayout->setSpacing(0);

    setContentsMargins(0, 0, 0, 0);
    QWidget* _MessageField = new QWidget();
    QVBoxLayout* messageLayout = new QVBoxLayout(_MessageField);


    _MessageField->setStyleSheet("background-color: #0e1621;");

    setStyleSheet("background-color: #0e1621;");
    chatScrollArea = new QScrollArea(chatListWidget);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    messageScrollArea = new QScrollArea(this);
    messageScrollArea->setWidgetResizable(true);
    messageScrollArea->setWidget(_MessageField);
    messageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _MessageField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    chatGridLayout->addWidget(chatScrollArea, 0, 0);
    chatScrollArea->setStyleSheet("border: none");
    chatGridLayout->addWidget(messageScrollArea, 0, 1);

    chatScrollArea->setFixedWidth(width() / 3);

    chatListWidget->setLayout(chatGridLayout);

    for (int i = 0; i < 100; i++)
        createChat(imagePath, "ChatGPT | Midjourney            ");
}


void ScrollBar::adjustTextWidth(QLabel* label)
{
    QFontMetrics metrics(label->font());
    QString elidedText = metrics.elidedText(label->text(), Qt::ElideRight, label->width());
    label->setText(elidedText);
}

void ScrollBar::createChat(const QString imagePath, const QString chatName)
{
    QListWidgetItem* chatItem = new QListWidgetItem(chatList);
    QWidget* chatWidget = new QWidget();
    QHBoxLayout* chatLayout = new QHBoxLayout(chatWidget);
    ClickableLabel* _IconChannelLabel = new ClickableLabel();
    QLabel* chatNameLabel = new QLabel(chatName);
    QPixmap pixmap1(imagePath);

    chatItem->setSizeHint(QSize(width() / 3, height() / 18));
    chatList->setItemWidget(chatItem, chatWidget);
    chatList->setStyleSheet("background-color: #17212b;");

    _IconChannelLabel->setFixedSize((height() / 18) * 0.8, (height() / 18) * 0.8);
    chatNameLabel->setFixedWidth(width() / 3 - (chatLayout->contentsMargins().left() + chatLayout->contentsMargins().right()) - _IconChannelLabel->width());
    adjustTextWidth(chatNameLabel);

    _IconChannelLabel->setBackgroundRole(QPalette::Dark);
    _IconChannelLabel->setScaledContents(true);
    _IconChannelLabel->setPixmap(pixmap1);
    _IconChannelLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _IconChannelLabel->setStyleSheet("background-color: transparent");
    _IconChannelLabel->setAttribute(Qt::WA_NoSystemBackground);
    _IconChannelLabel->setCursor(Qt::PointingHandCursor);
    QRegion region(QRect(0, 0, _IconChannelLabel->width() - _IconChannelLabel->width() * 0.01, _IconChannelLabel->height() - _IconChannelLabel->height() * 0.01), QRegion::Ellipse);
    _IconChannelLabel->setMask(region);

    chatLayout->addWidget(_IconChannelLabel);
    chatLayout->addWidget(chatNameLabel);

    chatGridLayout->addWidget(chatList,0, 0);
    chatScrollArea->setWidget(chatList);
}