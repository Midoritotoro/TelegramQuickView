#pragma once

#include <QWidget>

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class QGraphicsVideoItem;
class QMediaPlayer;


class AbstractMediaPlayer: public QWidget {
	Q_OBJECT
	QMediaPlayer* _mediaPlayer = nullptr;
	QGraphicsPixmapItem* _currentImageItem = nullptr;
	QGraphicsView* _videoView = nullptr;
	QGraphicsVideoItem* _videoItem = nullptr;
	QPointF _currentMediaPosition;
	QSizeF _currentMediaSize;
	bool _allowChangeVideoState = false;
public:
	explicit AbstractMediaPlayer(QWidget* parent = nullptr);

	void setSource(const QUrl& source);
	void clearScene();

	[[nodiscard]] static QString detectMediaType(const QString& filePath);
	[[nodiscard]] QSizeF occupiedMediaSpace() const noexcept;
	[[nodiscard]] QPointF mediaPosition() const noexcept;

	virtual [[nodiscard]] int getVideoControlsHeight() const noexcept = 0;
protected:
	[[nodiscard]] QMediaPlayer* mediaPlayer() const noexcept;

	void videoClicked();
	void adjustVideoSize();

	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void videoRewind(int value);
	void changeVolume(int value);
};