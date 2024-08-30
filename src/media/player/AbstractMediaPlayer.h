#pragma once

#include <QWidget>

class QMediaPlayer;

class AbstractMediaPlayer: public QWidget {
	Q_OBJECT
	QMediaPlayer* _mediaPlayer = nullptr;
	QGraphicsPixmapItem* _currentImageItem = nullptr;
	QPointF _currentMediaPosition;
	QSizeF _currentMediaSize;
public:
	explicit AbstractMediaPlayer(QWidget* parent = nullptr);

	void setSource(const QUrl& source);
	void clearScene();

	[[nodiscard]] static QString detectMediaType(const QString& filePath);
	[[nodiscard]] QSize occupiedMediaSpace() const noexcept;
	[[nodiscard]] QPoint mediaPosition() const noexcept;
protected:
	void videoClickedEvent();
	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
public Q_SLOTS:
	void setMediaPlayerVideoPosition(int value);
	void changeVolume(int value);
	void videoRewind(int value);
};