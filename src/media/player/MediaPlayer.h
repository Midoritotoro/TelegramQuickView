#pragma once

#include "../Manager.h"
#include "MediaPlayerPanel.h"

#include "WidgetsHider.h"

#include <QWidget>


class MediaPlayer final: public QWidget {
	Q_OBJECT
public:
	enum class MediaType {
		Image,
		Video,
		Audio,
		Unknown
	};

	MediaPlayer(QWidget* parent = nullptr);

	void setMedia(const QString& path);

	[[nodiscard]] int getVideoControlsHeight() const noexcept;

	[[nodiscard]] static MediaType detectMediaType(const QString& path);
	[[nodiscard]] QSize occupiedMediaSpace() const noexcept;
	[[nodiscard]] QPoint mediaPosition() const noexcept;

	[[nodiscard]] Manager::State playbackState() const noexcept;

	void setFullScreen();
	void setNormal();

	void play();
	void pause();

	void cleanUp();
	void rewind(Time::time positionMs);

	void changeVolume(int value);
Q_SIGNALS:
	void mediaGeometryChanged();
	void needScrollToMessage();
protected:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
private:
	enum class MediaDisplayType {
		FullScreen,
		Normal
	};

	void paintBackground(QPainter& painter, QPaintEvent* event);
	void updatePanelVisibility();

	[[nodiscard]] QImage prepareImage(const QImage& sourceImage);

	std::unique_ptr<Manager> _manager = nullptr;
	MediaPlayerPanel* _mediaPlayerPanel = nullptr;

	std::unique_ptr<WidgetsHider> _widgetsHider = nullptr;

	QImage _current;
	QRect _currentFrameRect;

	Manager::State _playbackState;

	Time::time _currMs;

	MediaDisplayType _displayType = MediaDisplayType::Normal;
	MediaType _currentMediaType = MediaType::Unknown;

	QString _currentMediaPath;
};