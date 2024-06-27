#include "ScrollBar.h"

#include "../media/ClickableLabel.h"

MessagesArea::MessagesArea(QWidget* parent) :
    QWidget(parent)
{
    QGridLayout* grid = new QGridLayout(this);
    _ChatView = new QListView();
    _ChatModel = new QStandardItemModel();

    grid->addWidget(_ChatView);

    _ChatModel->insertColumn(0);
    _ChatView->setModel(_ChatModel);
    _ChatView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    addMessage("QStandardItemModel.Убрать editable по умолчанию. « : Март 16, 2009, 16 : 45 » Здравствуйте. Для QTreeView я использую QStandardItemModel.По умолчанию все элементы можно редактировать.Как это убрать ?nКак менять флаги или установить возможность редактирования что - то не нашёл.");

    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    setWindowFlag(Qt::FramelessWindowHint);
    setGeometry(screenWidth - (width() * 0.7), 0, width() * 0.7, screenHeight);
}

void MessagesArea::addMessage(const QString& text)
{
    int newRow = _ChatModel->rowCount();

    QFont boldFont;
    boldFont.setBold(true);

    _ChatModel->insertRows(newRow, 1);
    _ChatModel->setData(_ChatModel->index(newRow, 0), text, Qt::DisplayRole);
    _ChatModel->setData(_ChatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);

    _ChatView->scrollToBottom();
}

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
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("color: rgba(1, 0, 0, 0)");

    QString currentPath = QCoreApplication::applicationDirPath();
    QDir assetsDir(currentPath + "/../../TelegramQuickView/assets/images");
    QDir cssDir(currentPath + "/../../TelegramQuickView/src/css/");
    QString chatBackgroundPath = assetsDir.absolutePath() + "/chat_background.jpg";
    QString imagePath = "C:\\Users\\danya\\Downloads\\avatar.jpg";
    QString ChannelsScrollAreaStyle = cssDir.absolutePath() + "/ChannelsScrollAreaStyle.css";
    QFile ChannelsScrollAreaStyleFile(ChannelsScrollAreaStyle);

    chatListWidget = new QWidget(this);
    chatGridLayout = new QGridLayout(chatListWidget);
    chatList = new QListWidget(chatListWidget);

    chatListWidget->setContentsMargins(0, 0, 0, 0);
    chatList->setAutoScroll(false);
    chatList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatGridLayout->setContentsMargins(0, 0, 0, 0);
    chatGridLayout->setSpacing(0);

    setContentsMargins(0, 0, 0, 0);
    setCentralWidget(chatListWidget);
    QWidget* _MessageField = new QWidget();
    QVBoxLayout* messageLayout = new QVBoxLayout(_MessageField);

    _MessageField->setStyleSheet("background-color: #0e1621;");

    setStyleSheet("background-color: #0e1621;");
    chatScrollArea = new QScrollArea(chatListWidget);
    chatScrollArea->setWidgetResizable(true);
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    messageScrollArea = new QScrollArea(this);
    messageScrollArea->setWidgetResizable(true);
    messageScrollArea->setWidget(_MessageField);
    messageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _MessageField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    chatScrollArea->setWidget(chatList);
    chatGridLayout->addWidget(chatScrollArea, 0, 0, 1, 1);
    chatScrollArea->setStyleSheet("border: none");
    chatGridLayout->addWidget(messageScrollArea, 0, 1, 1, 1);
    chatScrollArea->setContentsMargins(0, 0, 0, 0);
    chatScrollArea->setFixedWidth(width() / 3);

    chatListWidget->setLayout(chatGridLayout);

    for (int i = 0; i < 100; i++)
        createChat(imagePath, "ChatGPT | Midjourney | Gemini");

    if (ChannelsScrollAreaStyleFile.open(QFile::ReadOnly)) {
        chatScrollArea->setStyleSheet(ChannelsScrollAreaStyleFile.readAll());
        ChannelsScrollAreaStyleFile.close();
    }

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

    _IconChannelLabel->setFixedSize(50, 50);
    chatNameLabel->setFixedWidth(width() / 3 - (chatLayout->contentsMargins().left() + chatLayout->contentsMargins().right()) - _IconChannelLabel->width() - chatScrollArea->verticalScrollBar()->minimumWidth());
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

    QWidget* currentMessageField = new QWidget();
    QVBoxLayout* messageLayout = new QVBoxLayout(currentMessageField);
    currentMessageField->setStyleSheet("background-color: #0e1621;");

    QScrollArea* currentMessageScrollArea = new QScrollArea(this);

    currentMessageScrollArea->setWidgetResizable(true);
    currentMessageScrollArea->setWidget(currentMessageField);
    currentMessageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    currentMessageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    currentMessageField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    chatGridLayout->addWidget(currentMessageField, 0, 1, 1, 1);

    connect(_IconChannelLabel, &ClickableLabel::clicked, this, [this]() {
        on_channelLabelClicked();
        });
}

void ScrollBar::on_channelLabelClicked()
{
    QLayoutItem* item = chatGridLayout->itemAtPosition(0, 1);
    item->widget()->hide();

    ClickableLabel* channelLabel = (ClickableLabel*)sender();

}