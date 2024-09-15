﻿#include "AbstractMediaPlayer.h"


#include <QMimeDatabase>
#include <QAudioOutput>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsVideoItem>
#include <QGraphicsGridLayout>
#include <QApplication>
#include <QGridLayout>
#include <QMouseEvent>
#include <QVideoSink>
#include <QMediaMetaData>
#include <QVideoFrame>


AbstractMediaPlayer::AbstractMediaPlayer(QWidget* parent):
	QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);

	QGridLayout* grid = new QGridLayout(this);

	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);

	_mediaPlayer = new QMediaPlayer();
	QAudioOutput* audioOutput = new QAudioOutput();

	QGraphicsScene* videoScene = new QGraphicsScene();
	_videoView = new QGraphicsView(videoScene);

	_videoView->setContentsMargins(0, 0, 0, 0);

	grid->addWidget(_videoView);

	_videoItem = new QGraphicsVideoItem();

	_videoItem->setCursor(Qt::PointingHandCursor);

	_videoView->setScene(videoScene);

	videoScene->addItem(_videoItem);

	_videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoItem->setAspectRatioMode(Qt::KeepAspectRatio);

	_mediaPlayer->setAudioOutput(audioOutput);
	_mediaPlayer->setVideoOutput(_videoItem);
}

void AbstractMediaPlayer::setSource(const QUrl& source) {
	if (_mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PlayingState)
		_mediaPlayer->stop();

	QString sourcePath;

	source.path().at(0) == "/"[0] 
	? sourcePath = source.path().remove(0, 1)
	: sourcePath = source.path();

	QString mediaType = detectMediaType(sourcePath);

	clearScene();

	if (mediaType.contains("video")) {
		_mediaPlayer->setSource(source);
		_mediaPlayer->play();
		emit sourceChanged(QUrl(source));
	}
	else if (mediaType.contains("image")) {
		QPixmap pixmap(sourcePath);

		_currentImageItem = new QGraphicsPixmapItem();
		_currentImageItem->setPixmap(pixmap);

		const auto imageWidth = pixmap.width();
		const auto imageHeight = pixmap.height();

		updateCurrentImageRect(imageWidth, imageHeight);
		emit sourceChanged(QUrl(source));
	}
}

void AbstractMediaPlayer::clearScene() {
	QGraphicsScene* scene = _videoView->scene();
	QList<QGraphicsItem*> items = scene->items();

	foreach(QGraphicsItem * item, items) {
		if (qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
			scene->removeItem(item);
			delete item;
			item = nullptr;
		}
	}
}

void AbstractMediaPlayer::updateCurrentImageRect(int imageWidth, int imageHeight) {
	if (_currentImageItem->pixmap().width() > width() &&
		_currentImageItem->pixmap().height() > height()) {
		// Если изображение не помещается в экран, изменяем его размер, сохраняя соотношение сторон

		const auto scale = qMin(_videoView->width() / imageWidth,
						   _videoView->height() / imageHeight);

		_currentImageItem->setScale(scale);
	}

	_currentImageItem->setPos((_videoView->width() - imageWidth * _currentImageItem->scale()) / 2.,
					   (_videoView->height() - imageHeight * _currentImageItem->scale()) / 2.);

	_videoView->scene()->addItem(_currentImageItem);

	_currentMediaSize = _currentImageItem->boundingRect().size().toSize();
	_currentMediaPosition = _currentImageItem->pos().toPoint();
}

void AbstractMediaPlayer::mediaPlayerShowNormal() {
	_allowChangeVideoSize = true;
	adjustVideoSize();
}

void AbstractMediaPlayer::mediaPlayerShowFullScreen() {
	_allowChangeVideoSize = false;

	const auto currentVideoHeight = _videoItem->boundingRect().height();
	const auto currentVideoWidth = _videoItem->boundingRect().width();

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	const auto scale = qMin(screenWidth / currentVideoWidth,
							screenHeight / currentVideoHeight);

	const auto videoSize = QSizeF(currentVideoWidth * scale, currentVideoHeight * scale);
	const auto videoPosition = QPointF((screenWidth - videoSize.width()) / 2.,
									(screenHeight - videoSize.height()) / 2.);

	_videoItem->setSize(videoSize);
	_videoItem->setPos(videoPosition);

	_currentMediaSize = _videoItem->sceneBoundingRect().size().toSize();
	_currentMediaPosition = _videoItem->scenePos().toPoint();

	_videoView->scene()->setSceneRect(QRectF(0, 0, screenWidth, screenHeight));

	emit videoSizeChanged();
}

void AbstractMediaPlayer::adjustVideoSize() {
	if (_allowChangeVideoSize == false)
		return;

	if (_mediaPlayer->mediaStatus() == QMediaPlayer::MediaStatus::BufferedMedia) {
		const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
		const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();

		auto videoSize = _mediaPlayer->metaData().value(QMediaMetaData::Resolution).toSizeF();

		auto scale = qMin(screenWidth / videoSize.width(),
						  screenHeight / videoSize.height());

		if (videoSize.width() > videoSize.height() && videoSize.width() > (screenWidth * 0.6)) {
			const auto maximumWidthScale = (screenWidth * 0.6) / videoSize.width();
			scale = qMin(scale, maximumWidthScale);
		}

		videoSize = QSizeF(videoSize.width() * scale, videoSize.height() * scale);
		const auto videoPosition = QPointF((screenWidth - videoSize.width()) / 2., 
										  (screenHeight - videoSize.height()) / 2.);

		if (_videoItem != nullptr) {
			_videoItem->setSize(videoSize);
			_videoItem->setPos(videoPosition);

			_currentMediaSize = _videoItem->sceneBoundingRect().size().toSize();
			_currentMediaPosition = _videoItem->scenePos().toPoint();

			emit videoSizeChanged();
		}

		_videoView->scene()->setSceneRect(QRectF(videoPosition, videoSize));
	}
}

void AbstractMediaPlayer::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton && _videoItem->sceneBoundingRect().contains(event->pos().toPointF())) {
		emit videoClicked();
	}
}

void AbstractMediaPlayer::resizeEvent(QResizeEvent* event) {
	adjustVideoSize();

	if (_currentImageItem == nullptr)
		return;

	_Analysis_assume_(_currentImageItem != NULL)

	const auto imageWidth = _currentImageItem->pixmap().width();
	const auto imageHeight = _currentImageItem->pixmap().height();

	updateCurrentImageRect(imageWidth, imageHeight);
}

void AbstractMediaPlayer::closeEvent(QCloseEvent* event) {
	if (_mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PlayingState) {
		_mediaPlayer->setSource(QUrl());
		_mediaPlayer->stop();
	}

	clearScene();
	QWidget::closeEvent(event);
}

void AbstractMediaPlayer::videoRewind(int value) {
	if (_allowChangeVideoState)
		_mediaPlayer->setPosition(value);
}

void AbstractMediaPlayer::changeVolume(int value) {
	const auto linearVolume = QAudio::convertVolume(value / qreal(100),
							  QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
	_mediaPlayer->audioOutput()->setVolume(linearVolume);
}

QString AbstractMediaPlayer::detectMediaType(const QString& filePath) {
	return QMimeDatabase().mimeTypeForFile(filePath).name();
}

QSizeF AbstractMediaPlayer::occupiedMediaSpace() const noexcept {
	return _currentMediaSize;
}

QPointF AbstractMediaPlayer::mediaPosition() const noexcept {
	return _currentMediaPosition;
}

QMediaPlayer* AbstractMediaPlayer::mediaPlayer() const noexcept {
	return _mediaPlayer;
}
