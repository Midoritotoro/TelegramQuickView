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
	bool _allowChangeVideoState = true, _allowChangeVideoSize = true;
public:
	enum class MediaType {
		Image,
		Video,
		Audio,
		Unknown
	};

	explicit AbstractMediaPlayer(QWidget* parent = nullptr);

	void setSource(const QUrl& source);

	[[nodiscard]] static MediaType detectMediaType(const QString& filePath);
	[[nodiscard]] QSizeF occupiedMediaSpace() const noexcept;
	[[nodiscard]] QPointF mediaPosition() const noexcept;

	void mediaPlayerShowNormal();
	void mediaPlayerShowFullScreen();

	virtual [[nodiscard]] int getVideoControlsHeight() const noexcept = 0;
Q_SIGNALS:
	void videoClicked();
	void videoSizeChanged();

	void sourceChanged(const QUrl& media);
protected:
	[[nodiscard]] QMediaPlayer* mediaPlayer() const noexcept;

	void clearScene();
	void updateCurrentImageRect(int imageWidth, int imageHeight);

	void adjustVideoSize();

	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) = 0;
public Q_SLOTS:
	void videoRewind(int value);
	void changeVolume(int value);
};