#pragma once

#include "../core/ImagePushButton.h"
#include "DynamicScrollArea.h"
#include "DynamicImageButton.h"

class DynamicIconChannelButton: public DynamicImageButton
{
private:
    Q_OBJECT
private:
    QString m_channelName;
    DynamicScrollArea* m_dynamicScrollArea = nullptr;
    QLabel* m_label = nullptr;
public:
    DynamicIconChannelButton(const QString channelName, const QImage& image,  QWidget* parent = nullptr, bool rounded = false);
    QString const getText();
    DynamicScrollArea* const getLinkedScrollArea();
    void setScrollAreaVisible(bool visible);
    void setScrollAreaWidget(QWidget* widget);
    QLabel* const getLinkedLabel();
public slots:

};