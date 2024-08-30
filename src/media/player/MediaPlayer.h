#pragma once

#include <QAudioOutput>
#include <QGraphicsGridLayout>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QMediaPlayer>
#include <QToolButton>

#include "../EnhancedSlider.h"
#include "../VolumeClickableLabel.h"

#include <QDir>
#include <QGraphicsProxyWidget>
#include <QGridLayout>
#include <QMimeDatabase>

#include "../WidgetsHider.h"
#include "../ClickableLabel.h"
#include "./VideoStateWidget.h"
#include "AbstractMediaPlayer.h"
#include <QDialog>
#include <QPoint>
#include <QSize>

class MediaPlayer : public AbstractMediaPlayer
{
private:
	Q_OBJECT
public:
	MediaPlayer(QWidget* parent = nullptr);
};
