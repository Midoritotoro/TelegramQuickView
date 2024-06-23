#include "DynamicIconChannelButton.h"

DynamicIconChannelButton::DynamicIconChannelButton(const QString channelName, const QImage& image, QWidget* parent, bool rounded):
	DynamicImageButton(image, parent, rounded), m_channelName(channelName)
{
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScrollAreaWidth = ScreenWidth / 4.5;
	m_dynamicScrollArea = new DynamicScrollArea(parent);
	m_dynamicScrollArea->setFixedWidth(ScrollAreaWidth);
	m_label = new QLabel(channelName, parent);
}

QString const DynamicIconChannelButton::getText()
{
	return m_channelName;
}

DynamicScrollArea* const DynamicIconChannelButton::getLinkedScrollArea()
{
	return m_dynamicScrollArea;
}

void DynamicIconChannelButton::setScrollAreaVisible(bool visible)
{
	 visible ? m_dynamicScrollArea->show() : m_dynamicScrollArea->hide();
	 visible ? m_label->show() : m_label->hide();
}

void DynamicIconChannelButton::setScrollAreaWidget(QWidget* widget)
{
	m_dynamicScrollArea->setWidget(widget);
}

QLabel* const DynamicIconChannelButton::getLinkedLabel()
{
	return m_label;
}