#pragma once

#include <QWidget>

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMediaPlayer>

class QGraphicsVideoItem;


class AbstractMediaPlayer: public QWidget {
	Q_OBJECT
private:
	QMediaPlayer* _mediaPlayer = nullptr;
	QGraphicsPixmapItem* _currentImageItem = nullptr;
	QGraphicsView* _videoView = nullptr;
	QGraphicsVideoItem* _videoItem = nullptr;
	QPointF _currentMediaPosition;
	QSizeF _currentMediaSize;
	bool _allowChangeVideoState = true;
public:
	explicit AbstractMediaPlayer(QWidget* parent = nullptr);

	void setSource(const QUrl& source);

	[[nodiscard]] static QString detectMediaType(const QString& filePath);
	[[nodiscard]] QSizeF occupiedMediaSpace() const noexcept;
	[[nodiscard]] QPointF mediaPosition() const noexcept;

	virtual [[nodiscard]] int getVideoControlsHeight() const noexcept = 0;
protected:
	[[nodiscard]] QMediaPlayer* mediaPlayer() const noexcept;

	void clearScene();
	void updateCurrentImageRect(int imageWidth, int imageHeight);

	void videoClicked();
	void adjustVideoSize();

	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) = 0;
public Q_SLOTS:
	void videoRewind(int value);
	void changeVolume(int value);
};