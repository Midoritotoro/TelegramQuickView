#pragma once

#include <QtWidgets>
#include <QMediaPlayer>
#include <qaudiooutput.h>
#include <QSlider>
#include <QDir>
#include <QCoreApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include <Windows.h>
#include <QMimeData>

#include "../../core/WidgetsHider.h"
#include "../VolumeClickableLabel.h"
#include "../EnhancedSlider.h"
#include "./VideoStateWidget.h"

class MediaPlayer : public QWidget
{
private:
	Q_OBJECT
	QMediaPlayer* MediaPlayerWidget = nullptr;
	QAudioOutput* audioOutput = nullptr;
	QSlider* VideoSlider = nullptr, * AudioSlider = nullptr;
	QGraphicsView* GraphicsView = nullptr;
	QGraphicsScene* GraphicsScene = nullptr;
	QGraphicsVideoItem* GraphicsVideoItem = nullptr;
	QGraphicsGridLayout* grid = nullptr, * toolLayout = nullptr, * containerLayout = nullptr;
	QGraphicsWidget* form = nullptr, * containerWidget = nullptr, * toolWidget = nullptr, * GraphicsVolumeClickableLabel = nullptr,
				   * GraphicsAudioSlider = nullptr,* GraphicsVideoSlider = nullptr, * GraphicsVideoTimeLabel = nullptr;
	QToolButton* minButton = nullptr,* closeButton = nullptr, * maxButton = nullptr;
	QLabel* videoTimeLabel = nullptr;
	ClickableLabel *videoRepeatClickableLabel = nullptr, *videoStopClickableLabel = nullptr, *videoPlayClickableLabel = nullptr, * _VolumeClickableLabel = nullptr;
	QGraphicsWidget* GraphicsVideoStopButton = nullptr, * GraphicsVideoPlayButton = nullptr;
	VideoStateWidget* videoStateWidget = nullptr;
	bool doubleClicked = false, _allowChangeVideoState = true;
public:
	MediaPlayer(QWidget* parent = nullptr);
	void setSource(const QUrl& source);
	void play();
	void volumeChanged(int value);
	void videoSliderSetValue(int value);
	void setMediaPlayerVideoPosition(int value);
	void adjustVideoSize();
	QString detectMediaType(const QString& filePath);
protected:
	void videoClicked();
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void adjustBottomWidgets();
};
