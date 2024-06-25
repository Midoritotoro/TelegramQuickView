#include "ScrollBar.h"

#include "../media/ClickableLabel.h"


ScrollBar::ScrollBar(QWidget* parent) :
	QMainWindow(parent)
{
	CreateScrollBar();
}

void ScrollBar::CreateScrollBar() {
    resize(600, 400);

    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    setGeometry(screenWidth - width(), 0, width(), screenHeight);

    QString currentPath = QCoreApplication::applicationDirPath();
    QDir assetsDir(currentPath + "/../../TelegramQuickView/assets/images");
    QString chatBackgroundPath = assetsDir.absolutePath() + "/chat_background.jpg";
    setContentsMargins(0, 0, 0, 0);

    QWidget* chatListWidget = new QWidget(this);
    setCentralWidget(chatListWidget);
    chatListWidget->setContentsMargins(0, 0, 0, 0);
    QString imagePath = "C:\\Users\\danya\\Downloads\\avatar.jpg";
    QGridLayout* chatGridLayout = new QGridLayout(chatListWidget);
    chatGridLayout->setContentsMargins(0, 0, 0, 0);
    chatGridLayout->setSpacing(0);

    QListWidget* chatList = new QListWidget(chatListWidget);
    QListWidgetItem* chatItem1 = new QListWidgetItem(chatList);
    QWidget* chatWidget1 = new QWidget();
    QHBoxLayout* chatLayout1 = new QHBoxLayout(chatWidget1);

    ClickableLabel* _IconChannelLabel1 = new ClickableLabel();
    QLabel* nameLabel1 = new QLabel("ChatGPT4 | Midjourney fg g gg ggggggggggggg"); // Нужна обрезка текста при нехватке места
    _IconChannelLabel1->setFixedSize(50, 50);
    nameLabel1->setFixedWidth(width() / 3 - (chatLayout1->contentsMargins().left() + chatLayout1->contentsMargins().right()) - _IconChannelLabel1->width());
    QFontMetrics metrics(nameLabel1->font());
    QString elidedText = metrics.elidedText("ChatGPT4 | Midjourney fg g gg ggggggggggggg", Qt::ElideRight, nameLabel1->width());
    nameLabel1->setText(elidedText);


    QPixmap pixmap1(imagePath);
    _IconChannelLabel1->setBackgroundRole(QPalette::Dark);
    _IconChannelLabel1->setScaledContents(true);
    _IconChannelLabel1->setPixmap(pixmap1);
    _IconChannelLabel1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _IconChannelLabel1->setStyleSheet("background-color: transparent");
    _IconChannelLabel1->setAttribute(Qt::WA_NoSystemBackground);
    _IconChannelLabel1->setCursor(Qt::PointingHandCursor);
    QRegion region1(QRect(0, 0, _IconChannelLabel1->width() - _IconChannelLabel1->width() * 0.01, _IconChannelLabel1->height() - _IconChannelLabel1->height() * 0.01), QRegion::Ellipse);
    _IconChannelLabel1->setMask(region1);

    chatLayout1->addWidget(_IconChannelLabel1);
    chatLayout1->addWidget(nameLabel1);

    chatItem1->setSizeHint(QSize(width() / 3, height() / 18));
    chatList->setItemWidget(chatItem1, chatWidget1);

    QListWidgetItem* chatItem2 = new QListWidgetItem(chatList);
    QWidget* chatWidget2 = new QWidget();
    QHBoxLayout* chatLayout2 = new QHBoxLayout(chatWidget2);

    ClickableLabel* _IconChannelLabel2 = new ClickableLabel();
    QLabel* nameLabel2 = new QLabel("ChatGPT4 | Midjourney");
    QPixmap pixmap(imagePath);
    _IconChannelLabel2->setBackgroundRole(QPalette::Dark);
    _IconChannelLabel2->setScaledContents(true);
    _IconChannelLabel2->setPixmap(pixmap);
    _IconChannelLabel2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _IconChannelLabel2->setStyleSheet("background-color: transparent");
    _IconChannelLabel2->setAttribute(Qt::WA_NoSystemBackground);
    _IconChannelLabel2->setCursor(Qt::PointingHandCursor);
    _IconChannelLabel2->setFixedSize(50, 50);
    QRegion region2(QRect(0, 0, _IconChannelLabel2->width() - _IconChannelLabel2->width() * 0.01, _IconChannelLabel2->height() - _IconChannelLabel2->height() * 0.01), QRegion::Ellipse);
    _IconChannelLabel2->setMask(region2);

    chatLayout2->addWidget(_IconChannelLabel2);
    chatLayout2->addWidget(nameLabel2);

    chatItem2->setSizeHint(QSize(width() / 3, height() / 18));
    chatList->setItemWidget(chatItem2, chatWidget2);

    chatGridLayout->addWidget(chatList, 0, 0);

    QWidget* _MessageField = new QWidget();
    QVBoxLayout* messageLayout = new QVBoxLayout(_MessageField);

    chatList->setStyleSheet("background-color: #17212b;");
    _MessageField->setStyleSheet("background-color: #0e1621;");

    setStyleSheet("background-color: #0e1621;");
    QScrollArea* chatScrollArea = new QScrollArea(chatListWidget);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setWidget(chatList);
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QScrollArea* messageScrollArea = new QScrollArea(this);
    messageScrollArea->setWidgetResizable(true);
    messageScrollArea->setWidget(_MessageField);
    messageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _MessageField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    chatGridLayout->addWidget(chatScrollArea, 0, 0);
    chatScrollArea->setStyleSheet("border: none");
    chatGridLayout->addWidget(messageScrollArea, 0, 1);

    chatScrollArea->setFixedWidth(width() / 3);

    chatListWidget->setLayout(chatGridLayout);
}
