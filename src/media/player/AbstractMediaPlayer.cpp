#include "AbstractMediaPlayer.h"


#include <QMimeDatabase>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsVideoItem>
#include <QGraphicsGridLayout>
#include <QApplication>


AbstractMediaPlayer::AbstractMediaPlayer(QWidget* parent):
	QWidget(parent)
{
	setMouseTracking(true);

	_mediaPlayer = new QMediaPlayer();
	QAudioOutput* audioOutput = new QAudioOutput();

	QGraphicsScene* videoScene = new QGraphicsScene();
	_videoView = new QGraphicsView(videoScene);

	QGraphicsVideoItem* videoItem = new QGraphicsVideoItem();

	_videoView->setScene(videoScene);

	videoItem->setSize(size());
	videoScene->setSceneRect(QRectF(QPointF(0, 0), size()));

	videoScene->addItem(videoItem);

	_videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	videoItem->setAspectRatioMode(Qt::KeepAspectRatio);

	_mediaPlayer->setAudioOutput(audioOutput);
	_mediaPlayer->setVideoOutput(videoItem);
}

void AbstractMediaPlayer::setSource(const QUrl& source) {
	const int screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const int screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	QGraphicsVideoItem* mediaPlayerOutput = qobject_cast<QGraphicsVideoItem*>(_mediaPlayer->videoOutput());

	QString sourcePath;
	source.path().at(0) == "/"[0] ? sourcePath = source.path().remove(0, 1) : sourcePath = source.path();

	QString mediaType = detectMediaType(sourcePath);

	if (!_mediaPlayer->source().isEmpty()) {
		_mediaPlayer->stop();
		_mediaPlayer->setSource(QUrl());
	}

	if (mediaType.contains("video")) {
		clearScene();
		mediaPlayerOutput->setSize(QSizeF(screenWidth, screenHeight));

		_mediaPlayer->setSource(source);
		_mediaPlayer->play();

		_currentMediaSize = mediaPlayerOutput->boundingRect().size().toSize();
		_currentMediaPosition = mediaPlayerOutput->pos().toPoint();

	}
	else if (mediaType.contains("image")) {
		clearScene();
		QPixmap pixmap(sourcePath);

		_currentImageItem = new QGraphicsPixmapItem();
		_currentImageItem->setPixmap(pixmap);

		const int imageWidth = _currentImageItem->pixmap().width();
		const int imageHeight = _currentImageItem->pixmap().height();

		if (_currentImageItem->pixmap().width() > screenWidth &&
			_currentImageItem->pixmap().height() > screenHeight) {
			// Если изображение не помещается в экран, изменяем его размер, сохраняя соотношение сторон

			const qreal scale = qMin(_videoView->width() / (qreal)imageWidth, _videoView->height() / (qreal)imageHeight);
			_currentImageItem->setScale(scale);
		}

		_currentImageItem->setPos((_videoView->width() - imageWidth * _currentImageItem->scale()) / 2, (_videoView->height() - imageHeight * _currentImageItem->scale()) / 2);
		_videoView->scene()->addItem(_currentImageItem);

		_currentMediaSize = _currentImageItem->boundingRect().size().toSize();
		_currentMediaPosition = _currentImageItem->pos().toPoint();
	}
}

void AbstractMediaPlayer::clearScene() {
	QGraphicsScene* scene = _videoView->scene();
	QList<QGraphicsItem*> items = scene->items();

	if (items.count() < 1)
		return;

	foreach(QGraphicsItem * item, items) {
		if (qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
			scene->removeItem(item);
			delete item;
			item = nullptr;
		}
	}
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

void AbstractMediaPlayer::videoClicked() {
	if (!_allowChangeVideoState)
		return;

	if (_mediaPlayer->playbackState() == QMediaPlayer::PlayingState)
		_mediaPlayer->pause();
	else if (_mediaPlayer->playbackState() == QMediaPlayer::PausedState)
		_mediaPlayer->play();
}

void AbstractMediaPlayer::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)
		videoClicked();
	event->accept();
}

void AbstractMediaPlayer::resizeEvent(QResizeEvent* event) {
	Q_UNUSED(event);

	QGraphicsVideoItem* mediaPlayerOutput = qobject_cast<QGraphicsVideoItem*>(_mediaPlayer->videoOutput());

	if (!_currentImageItem) {
		_currentMediaSize = mediaPlayerOutput->boundingRect().size().toSize();
		_currentMediaPosition = mediaPlayerOutput->pos().toPoint();

		return;
	}

	if (_currentImageItem->pixmap().width() > QApplication::primaryScreen()->availableGeometry().width() &&
		_currentImageItem->pixmap().height() > QApplication::primaryScreen()->availableGeometry().height()) {
		// Если изображение не помещается в экран, изменяем его размер, сохраняя соотношение сторон

		qreal scale = qMin(_videoView->width() / (qreal)_currentImageItem->pixmap().width(), _videoView->height() / (qreal)_currentImageItem->pixmap().height());
		_currentImageItem->setScale(scale);
	}
	_currentImageItem->setPos((_videoView->width() - _currentImageItem->pixmap().width() * _currentImageItem->scale()) / 2, (_videoView->height() - _currentImageItem->pixmap().height() * _currentImageItem->scale()) / 2);

	_currentMediaSize = _currentImageItem->boundingRect().size().toSize();
	_currentMediaPosition = _currentImageItem->pos().toPoint();
}

void AbstractMediaPlayer::videoRewind(int value) {
	if (_allowChangeVideoState)
		_mediaPlayer->setPosition(value);
}

void AbstractMediaPlayer::changeVolume(int value) {
	qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
	_mediaPlayer->audioOutput()->setVolume(linearVolume);
}
