#include "MediaPlayer.h"


MediaPlayer::MediaPlayer(QWidget* parent) :
	QWidget(parent)
{
	setMouseTracking(true);

	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);

	setContentsMargins(0, 0, 0, 0);
	QGridLayout* gridLayout = new QGridLayout();

	_mediaPlayerWidget = new QMediaPlayer();
	_audioOutput = new QAudioOutput();

	setWindowFlag(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("color: rgba(1, 0, 0, 0)");

	minButton = new QToolButton();
	closeButton = new QToolButton();
	maxButton = new QToolButton();

	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	QPixmap maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);

	minButton->setIcon(minPix);
	closeButton->setIcon(closePix);
	maxButton->setIcon(maxPix);

	minButton->setAttribute(Qt::WA_NoSystemBackground);
	closeButton->setAttribute(Qt::WA_NoSystemBackground);
	maxButton->setAttribute(Qt::WA_NoSystemBackground);

	minButton->setFixedSize(screenWidth / 60, screenHeight / 40);
	closeButton->setFixedSize(screenWidth / 60, screenHeight / 40);
	maxButton->setFixedSize(screenWidth / 60, screenHeight / 40);

	minButton->setStyleSheet("background-color: transparent;");
	closeButton->setStyleSheet("background-color: transparent;");
	maxButton->setStyleSheet("background-color: transparent;");

	_videoScene = new	QGraphicsScene();
	_videoView = new QGraphicsView(_videoScene);
	_videoItem = new QGraphicsVideoItem();

	_videoForm = new QGraphicsWidget();
	_videoFormLayout = new QGraphicsGridLayout();

	_toolWidget = new QGraphicsWidget();
	_toolLayout = new QGraphicsGridLayout();

	_containerWidget = new QGraphicsWidget();
	_containerLayout = new QGraphicsGridLayout();

	_videoView->setScene(_videoScene);

	_videoItem->setSize(size());
	_videoScene->setSceneRect(QRectF(QPointF(0, 0), size()));

	_videoScene->addItem(_videoItem);

	_videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_videoItem->setAspectRatioMode(Qt::KeepAspectRatio);

	_videoSlider = new EnhancedSlider(Qt::Horizontal);
	_audioSlider = new EnhancedSlider(Qt::Horizontal);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../assets/images");

	QString speakerImagePath = assetsDir.absolutePath() + "/speaker_white.png";
	QString stopImagePath = assetsDir.absolutePath() + "/stop.png";
	QString playImagePath = assetsDir.absolutePath() + "/play.png";
	QString repeatImagePath = assetsDir.absolutePath() + "/repeat.png";

	_VolumeClickableLabel = new VolumeClickableLabel(speakerImagePath, _audioSlider);
	_VolumeClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);
	_audioSlider->setRange(0, 100);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setSpacing(0);

	videoTimeLabel = new QLabel();
	videoTimeLabel->setStyleSheet("background-color: transparent");
	videoTimeLabel->setAttribute(Qt::WA_NoSystemBackground);
	videoTimeLabel->setFixedSize(screenWidth / 30, screenHeight / 60);

	videoStopClickableLabel = new ClickableLabel();
	videoPlayClickableLabel = new ClickableLabel();
	videoRepeatClickableLabel = new ClickableLabel();

	QPixmap stopImagePixmap(stopImagePath);
	videoStopClickableLabel->setBackgroundRole(QPalette::Dark);
	videoStopClickableLabel->setScaledContents(true);
	videoStopClickableLabel->setPixmap(stopImagePixmap);
	videoStopClickableLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QPixmap playImagePixmap(playImagePath);
	videoPlayClickableLabel->setBackgroundRole(QPalette::Dark);
	videoPlayClickableLabel->setScaledContents(true);
	videoPlayClickableLabel->setPixmap(playImagePixmap);
	videoPlayClickableLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QPixmap repeatImagPixmap(repeatImagePath);
	videoRepeatClickableLabel->setBackgroundRole(QPalette::Dark);
	videoRepeatClickableLabel->setScaledContents(true);
	videoRepeatClickableLabel->setPixmap(repeatImagPixmap);
	videoRepeatClickableLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	videoStopClickableLabel->setStyleSheet("background-color: transparent");
	videoPlayClickableLabel->setStyleSheet("background-color: transparent");
	videoRepeatClickableLabel->setStyleSheet("background-color: transparent");

	videoStopClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);
	videoPlayClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);
	videoRepeatClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);

	videoStopClickableLabel->setAttribute(Qt::WA_NoSystemBackground);
	videoPlayClickableLabel->setAttribute(Qt::WA_NoSystemBackground);
	videoRepeatClickableLabel->setAttribute(Qt::WA_NoSystemBackground);

	videoStopClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);
	videoPlayClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);

	videoStateWidget = new VideoStateWidget(videoPlayClickableLabel, videoStopClickableLabel, videoRepeatClickableLabel);

	videoStateWidget->setStyleSheet("background-color: transparent");
	videoStateWidget->setFixedSize(screenWidth / 60, screenHeight / 40);

	QDir qssDir(currentPath + "/../../src/css/");
	QString VideoSliderStyle = qssDir.absolutePath() + "/VideoSliderStyle.css";
	QString AudioSliderStyle = qssDir.absolutePath() + "/AudioSliderStyle.css";

	QFile VideoSliderStyleFile(VideoSliderStyle);
	QFile AudioSliderStyleFile(AudioSliderStyle);

	if (VideoSliderStyleFile.open(QFile::ReadOnly) && AudioSliderStyleFile.open(QFile::ReadOnly)) {
		_videoSlider->setStyleSheet(VideoSliderStyleFile.readAll());
		_audioSlider->setStyleSheet(AudioSliderStyleFile.readAll());

		VideoSliderStyleFile.close();
		AudioSliderStyleFile.close();
	}

	_mediaPlayerWidget->setAudioOutput(_audioOutput);
	_mediaPlayerWidget->setVideoOutput(_videoItem);

	setLayout(gridLayout);

	gridLayout->addWidget(_videoView);

	_videoForm->setMaximumSize(screenWidth, screenHeight);
	_videoFormLayout->setMaximumSize(screenWidth, screenHeight);

	_toolWidget->setMaximumSize(screenWidth, screenHeight);
	_toolLayout->setMaximumSize(screenWidth, screenHeight);

	_toolWidget->setContentsMargins(0, 0, 0, 0);
	_toolLayout->setContentsMargins(0, 0, 0, 0);

	_videoForm->setGeometry(QRectF(_videoForm->pos().x(), _videoForm->pos().y(), size().width(), size().height()));
	_videoFormLayout->setGeometry(QRectF(_videoForm->pos().x(), _videoForm->pos().y(), size().width(), size().height()));

	_toolWidget->setGeometry(QRectF(_toolWidget->pos().x(), _toolWidget->pos().y(), size().width(), size().height()));
	_toolLayout->setGeometry(QRectF(_toolWidget->pos().x(), _toolWidget->pos().y(), size().width(), size().height()));

	_videoForm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_toolWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	_graphicsAudioSlider = _videoScene->addWidget(_audioSlider);
	QGraphicsWidget* volumeClickableLabel = _videoScene->addWidget(_VolumeClickableLabel);
	QGraphicsWidget* GraphicsVideoSlider = _videoScene->addWidget(_videoSlider);
	GraphicsVideoTimeLabel = _videoScene->addWidget(videoTimeLabel);
	QGraphicsWidget* GraphicsVideoStateWidget = _videoScene->addWidget(videoStateWidget);

	QGraphicsWidget* GraphicsMinButton = _videoScene->addWidget(minButton);
	QGraphicsWidget* GraphicsMaxButton = _videoScene->addWidget(maxButton);
	QGraphicsWidget* GraphicsCloseButton = _videoScene->addWidget(closeButton);

	_graphicsAudioSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsVideoSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	GraphicsVideoTimeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsVideoStateWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsMinButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsMaxButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsCloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	_containerWidget->setLayout(_containerLayout);
	_toolWidget->setLayout(_toolLayout);

	_containerLayout->setSpacing(screenWidth / 250);
	_containerWidget->setContentsMargins(0, 0, 0, 0);
	_videoFormLayout->setContentsMargins(0, 0, 0, 0);

	_containerLayout->addItem(GraphicsVideoSlider, 0, 0, 1, 5, Qt::AlignLeft | Qt::AlignBottom);
	_containerLayout->addItem(GraphicsVideoStateWidget, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	_containerLayout->addItem(volumeClickableLabel, 1, 1, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	_containerLayout->addItem(_graphicsAudioSlider, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	_containerLayout->addItem(GraphicsVideoTimeLabel, 1, 3, 1, 1, Qt::AlignLeft | Qt::AlignBottom);


	_toolLayout->addItem(GraphicsMinButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	_toolLayout->addItem(GraphicsMaxButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);
	_toolLayout->addItem(GraphicsCloseButton, 0, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);

	_containerLayout->setRowSpacing(1, _graphicsAudioSlider->size().height() * 2);
	_toolLayout->setHorizontalSpacing(0);

	_containerLayout->setContentsMargins(10, 10, 10, 10);

	_videoView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_videoFormLayout->addItem(_toolWidget, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	_videoFormLayout->addItem(_containerWidget, 1, 0, 1, 1, Qt::AlignBottom);

	_videoForm->setLayout(_videoFormLayout);
	_videoScene->addItem(_videoForm);

	connect(_videoSlider, &QSlider::sliderPressed, [this]() {
		disconnect(_mediaPlayerWidget, &QMediaPlayer::positionChanged, _videoSlider, &QSlider::setValue);
		_mediaPlayerWidget->pause();
		});

	connect(_videoSlider, &QSlider::sliderReleased, [this]() {
		connect(_mediaPlayerWidget, &QMediaPlayer::positionChanged, _videoSlider, &QSlider::setValue);
		Sleep(1);
		_mediaPlayerWidget->play();
		});

	connect(_mediaPlayerWidget, &QMediaPlayer::durationChanged, _videoSlider, &QSlider::setMaximum);
	connect(_mediaPlayerWidget, &QMediaPlayer::positionChanged, this, &MediaPlayer::videoSliderSetValue);
	connect(_mediaPlayerWidget, &QMediaPlayer::mediaStatusChanged, this, [this]() {
		if (_mediaPlayerWidget->mediaStatus() == _mediaPlayerWidget->EndOfMedia) {
			videoStateWidget->stackedWidget()->setCurrentIndex(2);
		}
		});

	connect(_videoSlider, &QSlider::sliderMoved, this, &MediaPlayer::setMediaPlayerVideoPosition);
	connect(_audioSlider, &QSlider::valueChanged, this, &MediaPlayer::volumeChanged);

	connect(_VolumeClickableLabel, &ClickableLabel::clicked, this, [this]() {
		if (_audioSlider->isHidden())
			_audioSlider->show();
		else
			_audioSlider->hide();
		adjustBottomWidgets();
		});

	connect(minButton, &QToolButton::clicked, this, &QWidget::showMinimized);
	connect(closeButton, &QToolButton::clicked, this, &QWidget::close);
	connect(maxButton, &QToolButton::clicked, this, [this]() {
		isFullScreen() ? showNormal() : showFullScreen();
		});

	connect(videoPlayClickableLabel, &ClickableLabel::clicked, this, [this]() {
		videoStateWidget->stackedWidget()->setCurrentIndex(0);
		videoClicked();
		});
	connect(videoStopClickableLabel, &ClickableLabel::clicked, this, [this]() {
		videoStateWidget->stackedWidget()->setCurrentIndex(1);
		videoClicked();
		});
	connect(videoRepeatClickableLabel, &ClickableLabel::clicked, this, [this]() {
		_mediaPlayerWidget->setPosition(0);
		_allowChangeVideoState = true;
		if (_mediaPlayerWidget->playbackState() != _mediaPlayerWidget->PlayingState) {
			_mediaPlayerWidget->play();
		}
		videoStateWidget->stackedWidget()->setCurrentIndex(0);
		});

	QWidgetList widgetsList = QWidgetList({ _videoSlider, _VolumeClickableLabel, videoStateWidget, videoTimeLabel, minButton, maxButton, closeButton });
	WidgetsHider& widgetsHider = WidgetsHider::Instance(widgetsList);
	widgetsHider.SetInactivityDuration(3000);

	connect(&widgetsHider, &WidgetsHider::widgetsShowed, this, &MediaPlayer::adjustBottomWidgets);
	connect(&widgetsHider, &WidgetsHider::widgetsHidden, this, &MediaPlayer::adjustBottomWidgets);

	_videoSlider->hide();
	_VolumeClickableLabel->hide();
	videoStateWidget->hide();
	videoTimeLabel->hide();
	minButton->hide();
	maxButton->hide(); 
	closeButton->hide();

	_audioSlider->setValue(50);
}

void MediaPlayer::volumeChanged(int value) {
	qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
	_audioOutput->setVolume(linearVolume);
}

void MediaPlayer::videoSliderSetValue(int value) {
	int position = _mediaPlayerWidget->position();
	int duration = _mediaPlayerWidget->duration();

	int positionSeconds = (position / 1000) % 60;
	int positionMinutes = (position / 1000) / 60;

	videoTimeLabel->setText(QString("%1:%2 / %3:%4")
		.arg(positionMinutes, 2, 10, QChar('0'))
		.arg(positionSeconds, 2, 10, QChar('0'))
		.arg(duration / 60000)
		.arg((duration / 1000) % 60));

	int timeToEnd = duration - position;
	if (timeToEnd <= 1000 && timeToEnd >= 0) {
		_mediaPlayerWidget->pause();
		setMediaPlayerVideoPosition(duration-1);
		_allowChangeVideoState = false;
		videoStateWidget->stackedWidget()->setCurrentIndex(2);
		return;
	}

	if (videoStateWidget->stackedWidget()->currentIndex() == 2 && _allowChangeVideoState == false) {
		_allowChangeVideoState = true;
		_mediaPlayerWidget->play();
		videoStateWidget->stackedWidget()->setCurrentIndex(0);
	}
	
	_videoSlider->setValue(value);
}

void MediaPlayer::setMediaPlayerVideoPosition(int value) {
	if (_allowChangeVideoState)
		_mediaPlayerWidget->setPosition(value);
}

void MediaPlayer::mouseDoubleClickEvent(QMouseEvent* event) {
	doubleClicked = true;
	mousePressEvent(event);

	if (isMaximized() || isFullScreen())
		showNormal();
	else
		showFullScreen();

	adjustVideoSize();
	doubleClicked = false;
	event->accept();
}

void MediaPlayer::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		videoClicked();
	}
	event->accept();
}


void MediaPlayer::videoClicked() {
	if (!_allowChangeVideoState)
		return;

	if (_mediaPlayerWidget->playbackState() == _mediaPlayerWidget->PlayingState) {
		_mediaPlayerWidget->pause();
	}
	else if (_mediaPlayerWidget->playbackState() == _mediaPlayerWidget->PausedState) {
		_mediaPlayerWidget->play();
	}
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	Q_UNUSED(event);

	_videoForm->setGeometry(QRectF(_videoForm->pos().x(), _videoForm->pos().y(), size().width(), size().height()));
	_videoFormLayout->setGeometry(QRectF(_videoForm->pos().x(), _videoForm->pos().y(), size().width(), size().height()));

	_toolWidget->setGeometry(QRectF(_toolWidget->pos().x(), _toolWidget->pos().y(), size().width(), size().height()));
	_toolLayout->setGeometry(QRectF(_toolWidget->pos().x(), _toolWidget->pos().y(), size().width(), size().height()));
	if (doubleClicked)
		return;

	adjustVideoSize();

	if (windowState() != Qt::WindowMaximized && windowState() != Qt::WindowFullScreen)
		return;

	isFullScreen() ? showNormal() : showFullScreen();
}

void MediaPlayer::adjustVideoSize() {
	_videoItem->setSize(size());
	_videoScene->setSceneRect(QRectF(QPointF(0, 0), size()));
}

void MediaPlayer::adjustBottomWidgets() {
	if (_audioSlider->isHidden()) {
		_containerLayout->removeItem(GraphicsVideoTimeLabel);
		QGraphicsLayoutItem* item = _containerLayout->itemAt(1, 2);

		if (item == _graphicsAudioSlider) {
			_containerLayout->removeItem(_graphicsAudioSlider);
			_containerLayout->addItem(_graphicsAudioSlider, 1, 3, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
		}

		_containerLayout->addItem(GraphicsVideoTimeLabel, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	}
	else {
		_containerLayout->removeItem(GraphicsVideoTimeLabel);
		QGraphicsLayoutItem* item = _containerLayout->itemAt(1, 3);

		if (item == _graphicsAudioSlider) {
			_containerLayout->removeItem(_graphicsAudioSlider);
			_containerLayout->addItem(_graphicsAudioSlider, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
		}

		_containerLayout->addItem(GraphicsVideoTimeLabel, 1, 3, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	}
}

QString MediaPlayer::detectMediaType(const QString& filePath) {
	return QMimeDatabase().mimeTypeForFile(filePath).name();
}

void MediaPlayer::setSource(const QUrl& source) {
	QString sourcePath;
	if (source.path().at(0) == "/"[0])
		sourcePath = source.path().remove(0, 1);
	else
		sourcePath = source.path();

	QString mediaType = detectMediaType(sourcePath);
	if (mediaType.contains("video")) {
		QList<QGraphicsItem*> items = _videoScene->items();
		for (QGraphicsItem* item : items) {
			if (qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
				_videoScene->removeItem(item);
				delete item;
			}
		}
		_containerWidget->show();
		_mediaPlayerWidget->setSource(source);
		_videoItem->setSize(QSize(464, 848));
		play();
	}
	else if (mediaType.contains("image")) {
		_containerWidget->hide();
		QGraphicsPixmapItem* m_currentImageItem = new QGraphicsPixmapItem(QPixmap(sourcePath));
		_videoScene->addItem(m_currentImageItem);
	}
}

void MediaPlayer::play() {
	_mediaPlayerWidget->play();
}
