#include "AbstractMediaPlayer.h"


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
	videoScene->addItem(_videoItem);

	_videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoItem->setAspectRatioMode(Qt::KeepAspectRatio);

	_mediaPlayer->setAudioOutput(audioOutput);
	_mediaPlayer->setVideoOutput(_videoItem);
}

void AbstractMediaPlayer::setSource(const QUrl& source) {
	QString sourcePath;

	source.path().at(0) == "/"[0] 
	? sourcePath = source.path().remove(0, 1)
	: sourcePath = source.path();

	MediaType mediaType = detectMediaType(sourcePath);

	clearScene();

	switch (mediaType) {
		case MediaType::Video:
			_mediaPlayer->setSource(source);
			_mediaPlayer->play();

			emit sourceChanged(source);
			break;

		case MediaType::Image: {
			QPixmap pixmap(sourcePath);

			_currentImageItem = new QGraphicsPixmapItem(pixmap);

			const auto imageWidth = pixmap.width();
			const auto imageHeight = pixmap.height();

			updateCurrentImageRect(imageWidth, imageHeight);

			emit sourceChanged(source);

			break;
		}

		case MediaType::Audio:
			break;
	}
}

void AbstractMediaPlayer::clearScene() {
	if (_mediaPlayer->source().isEmpty() == false)
		_mediaPlayer->setSource(QUrl());

	QGraphicsScene* scene = _videoView->scene();
	QList<QGraphicsItem*> items = scene->items();

	foreach(auto item, items) {
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

	_videoView->scene()->setSceneRect(QRectF(0, 0, screenWidth, screenHeight));

	_currentMediaSize = _videoItem->boundingRect().size().toSize();
	_currentMediaPosition = _videoItem->pos().toPoint();

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

		if (videoSize.width() * scale > (screenWidth * 0.6)) {
			const auto maximumWidthScale = (screenWidth * 0.6) / videoSize.width();
			scale = qMin(scale, maximumWidthScale);
		}

		videoSize = QSizeF(videoSize.width() * scale, videoSize.height() * scale);
		const auto videoPosition = QPointF((screenWidth - videoSize.width()) / 2., 
										  (screenHeight - videoSize.height()) / 2.);

		_videoView->scene()->setSceneRect(QRectF(videoPosition, videoSize));

		if (_videoItem != nullptr) {
			_videoItem->setSize(videoSize);
			_videoItem->setPos(videoPosition);

			_currentMediaSize = _videoItem->boundingRect().size().toSize();
			_currentMediaPosition = _videoItem->pos().toPoint();
		}
	}

	emit videoSizeChanged();
}

void AbstractMediaPlayer::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton && _videoItem->boundingRect().contains(event->pos().toPointF())) {
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

AbstractMediaPlayer::MediaType AbstractMediaPlayer::detectMediaType(const QString& filePath) {
	const auto mimeType = QMimeDatabase().mimeTypeForFile(filePath).name();

	if (mimeType.contains("video"))
		return MediaType::Video;
	else if (mimeType.contains("image"))
		return MediaType::Image;
	else if (mimeType.contains("audio"))
		return MediaType::Audio;

	return MediaType::Unknown;
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
