#include "AbstractMediaPlayer.h"


#include <QMimeDatabase>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsVideoItem>
#include <QGraphicsGridLayout>
#include <QApplication>
#include <QGridLayout>
#include <QMouseEvent>


AbstractMediaPlayer::AbstractMediaPlayer(QWidget* parent):
	QWidget(parent)
{
	QGridLayout* grid = new QGridLayout(this);

	_mediaPlayer = new QMediaPlayer();
	QAudioOutput* audioOutput = new QAudioOutput();

	QGraphicsScene* videoScene = new QGraphicsScene();
	_videoView = new QGraphicsView(videoScene);

	grid->addWidget(_videoView);

	_videoItem = new QGraphicsVideoItem();

	_videoView->setScene(videoScene);

	_videoItem->setSize(size());
	videoScene->setSceneRect(QRectF(QPointF(0, 0), size()));

	videoScene->addItem(_videoItem);

	_videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoItem->setAspectRatioMode(Qt::KeepAspectRatio);

	_mediaPlayer->setAudioOutput(audioOutput);
	_mediaPlayer->setVideoOutput(_videoItem);

	connect(_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
		if (status == QMediaPlayer::MediaStatus::EndOfMedia) {
			_mediaPlayer->pause();
			const auto duration = _mediaPlayer->duration();

			videoRewind(duration - 100);
			//_allowChangeVideoState = false;
			//_videoStateWidget->stackedWidget()->setCurrentIndex(2);
		}
		});
}

void AbstractMediaPlayer::setSource(const QUrl& source) {
	if (_mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PlayingState) {
		_mediaPlayer->stop();
	}

	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();


	QString sourcePath;

	source.path().at(0) == "/"[0] 
	? sourcePath = source.path().remove(0, 1)
	: sourcePath = source.path();

	QString mediaType = detectMediaType(sourcePath);

	if (mediaType.contains("video")) {
		clearScene();
		_videoItem->setSize(QSizeF(screenWidth, screenHeight));

		_mediaPlayer->setSource(source);
		_mediaPlayer->play();

		_currentMediaSize = _videoItem->boundingRect().size().toSize();
		_currentMediaPosition = _videoItem->pos().toPoint();

	}
	else if (mediaType.contains("image")) {
		clearScene();
		QPixmap pixmap(sourcePath);

		_currentImageItem = new QGraphicsPixmapItem();
		_currentImageItem->setPixmap(pixmap);

		const auto imageWidth = _currentImageItem->pixmap().width();
		const auto imageHeight = _currentImageItem->pixmap().height();

		updateCurrentImageRect(imageWidth, imageHeight);
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

void AbstractMediaPlayer::updateCurrentImageRect(int imageWidth, int imageHeight) {
	const auto screenWidth = QApplication::primaryScreen()->availableGeometry().width();
	const auto screenHeight = QApplication::primaryScreen()->availableGeometry().height();

	if (_currentImageItem->pixmap().width() > screenWidth &&
		_currentImageItem->pixmap().height() > screenHeight) {
		// Если изображение не помещается в экран, изменяем его размер, сохраняя соотношение сторон

		const qreal scale = qMin(_videoView->width() / (qreal)imageWidth,
			_videoView->height() / (qreal)imageHeight);

		_currentImageItem->setScale(scale);
	}

	_currentImageItem->setPos((_videoView->width() - imageWidth * _currentImageItem->scale()) / 2,
		(_videoView->height() - imageHeight * _currentImageItem->scale()) / 2);

	_videoView->scene()->addItem(_currentImageItem);

	_currentMediaSize = _currentImageItem->boundingRect().size().toSize();
	_currentMediaPosition = _currentImageItem->pos().toPoint();
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

void AbstractMediaPlayer::adjustVideoSize() {
	if (_currentImageItem == nullptr
		&& _videoItem != nullptr)
		_videoItem->setSize(size());
	_videoView->scene()->setSceneRect(QRectF(QPointF(0, 0), size()));
}

void AbstractMediaPlayer::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton)
		videoClicked();
	event->accept();
}

void AbstractMediaPlayer::resizeEvent(QResizeEvent* event) {
	adjustVideoSize();

	if (event->size() == size())
		return;

	if (!_currentImageItem && _videoItem != nullptr) {
		_currentMediaSize = _videoItem->boundingRect().size().toSize();
		_currentMediaPosition = _videoItem->pos().toPoint();

		return;
	}

	_Analysis_assume_(_currentImageItem != NULL) // Отключение предупреждения

	const auto imageWidth = _currentImageItem->pixmap().width();
	const auto imageHeight = _currentImageItem->pixmap().height();

	updateCurrentImageRect(imageWidth, imageHeight);
}

void AbstractMediaPlayer::videoRewind(int value) {
	if (_allowChangeVideoState)
		_mediaPlayer->setPosition(value);
}

void AbstractMediaPlayer::changeVolume(int value) {
	qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
	_mediaPlayer->audioOutput()->setVolume(linearVolume);
}