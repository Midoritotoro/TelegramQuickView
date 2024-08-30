#include "AbstractMediaPlayer.h"

#include <QMimeDatabase>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QGraphicsGridLayout>
#include <QApplication>


AbstractMediaPlayer::AbstractMediaPlayer(QWidget* parent):
	QWidget(parent)
{
	_mediaPlayer = new QMediaPlayer();
	QAudioOutput* audioOutput = new QAudioOutput();

	QGraphicsScene* videoScene = new QGraphicsScene();
	QGraphicsView* videoView = new QGraphicsView(videoScene);

	QGraphicsVideoItem* videoItem = new QGraphicsVideoItem();

	videoView->setScene(videoScene);

	videoItem->setSize(size());
	videoScene->setSceneRect(QRectF(QPointF(0, 0), size()));

	videoScene->addItem(videoItem);

	videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	videoItem->setAspectRatioMode(Qt::KeepAspectRatio);

	_mediaPlayer->setAudioOutput(audioOutput);
	_mediaPlayer->setVideoOutput(videoItem);
}

void AbstractMediaPlayer::setSource(const QUrl& source) {
	const int screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const int screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	QString sourcePath;
	if (source.path().at(0) == "/"[0])
		sourcePath = source.path().remove(0, 1);
	else
		sourcePath = source.path();

	QString mediaType = detectMediaType(sourcePath);

	if (!_mediaPlayer->source().isEmpty()) {
		_mediaPlayer->stop();
		_mediaPlayer->setSource(QUrl());
	}

	if (mediaType.contains("video")) {
		clearScene();

		_mediaPlayer->setSource(source);
		QGraphicsVideoItem* mediaPlayerOutput = qobject_cast<QGraphicsVideoItem*>(_mediaPlayer->videoOutput());

		mediaPlayerOutput->setSize(QSizeF(screenWidth, screenHeight));

		_mediaPlayer->play();

		_currentMediaSize = mediaPlayerOutput->boundingRect().size().toSize();
		_currentMediaPosition = mediaPlayerOutput->pos().toPoint();

	}
	else if (mediaType.contains("image")) {
		clearScene();

		QPixmap pixmap(sourcePath);

		_currentImageItem = new QGraphicsPixmapItem();
		_currentImageItem->setPixmap(pixmap);

		int imageWidth = _currentImageItem->pixmap().width();
		int imageHeight = _currentImageItem->pixmap().height();

		if (_currentImageItem->pixmap().width() > screenWidth &&
			_currentImageItem->pixmap().height() > screenHeight) {
			// Если изображение не помещается в экран, изменяем его размер, сохраняя соотношение сторон

			qreal scale = qMin(_videoView->width() / (qreal)imageWidth, _videoView->height() / (qreal)imageHeight);
			_currentImageItem->setScale(scale);
		}

		_currentImageItem->setPos((_videoView->width() - imageWidth * _currentImageItem->scale()) / 2, (_videoView->height() - imageHeight * _currentImageItem->scale()) / 2);
		_videoView->scene()->addItem(_currentImageItem);

		_currentMediaSize = _currentImageItem->boundingRect().size().toSize();
		_currentMediaPosition = _currentImageItem->pos().toPoint();
	}
}

void AbstractMediaPlayer::clearScene() {

}

QString AbstractMediaPlayer::detectMediaType(const QString& filePath) {
	return QMimeDatabase().mimeTypeForFile(filePath).name();
}

QSize AbstractMediaPlayer::occupiedMediaSpace() const noexcept {

}

QPoint AbstractMediaPlayer::mediaPosition() const noexcept {

}

void AbstractMediaPlayer::videoClickedEvent() {

}

void AbstractMediaPlayer::mousePressEvent(QMouseEvent* event) {

}

void AbstractMediaPlayer::resizeEvent(QResizeEvent* event) {

}

void AbstractMediaPlayer::setMediaPlayerVideoPosition(int value) {

}

void AbstractMediaPlayer::changeVolume(int value) {

}

void AbstractMediaPlayer::videoRewind(int value) {

}