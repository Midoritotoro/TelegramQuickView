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
#include <QDialog>
#include <QPoint>
#include <QSize>

class MediaPlayer : public QWidget
{
private:
	Q_OBJECT
	QMediaPlayer* _mediaPlayer = nullptr;
	QAudioOutput* _audioOutput = nullptr;
	EnhancedSlider* _videoSlider = nullptr, *_audioSlider = nullptr;
	QGraphicsView* _videoView = nullptr;
	QGraphicsScene* _videoScene = nullptr;
	QGraphicsVideoItem* _videoItem = nullptr;
	QGraphicsGridLayout* _videoFormLayout = nullptr, * _containerLayout = nullptr;
	QGraphicsWidget* _videoForm = nullptr, * _containerWidget = nullptr,
				   * _graphicsAudioSlider = nullptr, * _graphicsVideoTimeLabel = nullptr;
	QLabel* _videoTimeLabel = nullptr;
	VideoStateWidget* _videoStateWidget = nullptr;
	QGraphicsPixmapItem* _currentImageItem = nullptr;
	QSize _currentMediaSize;
	QPoint _currentMediaPosition;
	bool _doubleClicked = false, _allowChangeVideoState = true;
public:
	MediaPlayer(QWidget* parent = nullptr);

	void setSource(const QUrl& source);
	void adjustVideoSize();
	void adjustBottomWidgets();

	[[nodiscard]] static QString detectMediaType(const QString& filePath);
	void clearScene();
	[[nodiscard]] QSize occupiedMediaSpace() const noexcept;
	[[nodiscard]] QPoint mediaPosition() const noexcept;
	[[nodiscard]] int getVideoControlsHeight() const noexcept;
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
