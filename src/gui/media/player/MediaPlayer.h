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

#include "../../core/WidgetsHider.h"
#include "../ClickableLabel.h"
#include "./VideoStateWidget.h"
#include <QDialog>

class MediaPlayer : public QDialog
{
private:
	Q_OBJECT
	QMediaPlayer* _mediaPlayer = nullptr;
	QAudioOutput* _audioOutput = nullptr;
	EnhancedSlider* _videoSlider = nullptr, *_audioSlider = nullptr;
	QGraphicsView* _videoView = nullptr;
	QGraphicsScene* _videoScene = nullptr;
	QGraphicsVideoItem* _videoItem = nullptr;
	QGraphicsGridLayout* _videoFormLayout = nullptr, * _toolLayout = nullptr, * _containerLayout = nullptr;
	QGraphicsWidget* _videoForm = nullptr, * _containerWidget = nullptr, * _toolWidget = nullptr,
				   * _graphicsAudioSlider = nullptr, * _graphicsVideoTimeLabel = nullptr;
	QLabel* _videoTimeLabel = nullptr;
	VideoStateWidget* _videoStateWidget = nullptr;
	QGraphicsPixmapItem* _currentImageItem = nullptr;
	bool _doubleClicked = false, _allowChangeVideoState = true;
public:
	MediaPlayer(QWidget* parent = nullptr);

	void setSource(const QUrl& source);
	void adjustVideoSize();
	void adjustBottomWidgets();

	[[nodiscard]] QString detectMediaType(const QString& filePath);
protected:
	void videoClicked();
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void play();
	void stop();
	void pause();
	void setMediaPlayerVideoPosition(int value);
	void volumeChanged(int value);
	void videoSliderSetValue(int value);
};
