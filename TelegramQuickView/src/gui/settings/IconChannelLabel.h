#pragma once

#include <QScrollArea>

#include "../media/ClickableLabel.h"


class IconChannelLabel: public ClickableLabel
{
private:
    Q_OBJECT
private:
    QString m_channelName;
    QScrollArea* _ScrollArea = nullptr;
    QLabel* m_label = nullptr;
public:
    IconChannelLabel(const QString channelName, const QString imagePath,  QWidget* parent = nullptr);
    QString const getText();
    QScrollArea* const getLinkedScrollArea();
    void setScrollAreaVisible(bool visible);
    void setScrollAreaWidget(QWidget* widget);
    QLabel* const getLinkedLabel();
public slots:

};
