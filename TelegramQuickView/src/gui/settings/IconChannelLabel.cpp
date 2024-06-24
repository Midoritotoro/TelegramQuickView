#include "IconChannelLabel.h"
#include <windows.h>

IconChannelLabel::IconChannelLabel(const QString channelName, const QString imagePath, QWidget* parent) :
	ClickableLabel(parent), m_channelName(channelName)
{
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScrollAreaWidth = ScreenWidth / 4.5; //			426 / 640

	QPixmap pixmap(imagePath);
	setBackgroundRole(QPalette::Dark);
	setScaledContents(true);
	setPixmap(pixmap);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setStyleSheet("background-color: transparent");
	setAttribute(Qt::WA_NoSystemBackground);
	setCursor(Qt::PointingHandCursor);

	_ScrollArea = new QScrollArea();
	_ScrollArea->setFixedWidth(ScrollAreaWidth);

	_ScrollArea->setWidgetResizable(true);
	_ScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	m_label = new QLabel(channelName, _ScrollArea);
}

QString const IconChannelLabel::getText()
{
	return m_channelName;
}

QScrollArea* const IconChannelLabel::getLinkedScrollArea()
{
	return _ScrollArea;
}

void IconChannelLabel::setScrollAreaVisible(bool visible)
{
	 visible ? _ScrollArea->show() : _ScrollArea->hide();
	 visible ? m_label->show() : m_label->hide();
}

void IconChannelLabel::setScrollAreaWidget(QWidget* widget)
{
	_ScrollArea->setWidget(widget);
}

QLabel* const IconChannelLabel::getLinkedLabel()
{
	return m_label;
}