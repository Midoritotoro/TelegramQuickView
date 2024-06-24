#include "MediaPlayer.h"
#define VideoBegin 0



MediaPlayer::MediaPlayer(QWidget* parent) :
	QWidget(parent)
{
	setMouseTracking(true);

	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);

	setContentsMargins(0, 0, 0, 0);
	QGridLayout* gridLayout = new QGridLayout();

	MediaPlayerWidget = new QMediaPlayer();
	audioOutput = new QAudioOutput();

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

	GraphicsScene = new	QGraphicsScene();
	GraphicsView = new QGraphicsView(GraphicsScene);
	GraphicsVideoItem = new QGraphicsVideoItem();

	form = new QGraphicsWidget();
	grid = new QGraphicsGridLayout();

	toolWidget = new QGraphicsWidget();
	toolLayout = new QGraphicsGridLayout();

	containerWidget = new QGraphicsWidget();
	containerLayout = new QGraphicsGridLayout();

	GraphicsView->setScene(GraphicsScene);

	GraphicsVideoItem->setSize(size());
	GraphicsScene->setSceneRect(QRectF(QPointF(0, 0), size()));

	GraphicsScene->addItem(GraphicsVideoItem);

	GraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	GraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	GraphicsVideoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);

	VideoSlider = new EnhancedSlider(Qt::Horizontal);
	AudioSlider = new EnhancedSlider(Qt::Horizontal);

	QString currentPath = QCoreApplication::applicationDirPath();
	QDir assetsDir(currentPath + "/../../TelegramQuickView/assets/images");

	QString speakerImagePath = assetsDir.absolutePath() + "/speaker_white.png";
	QString stopImagePath = assetsDir.absolutePath() + "/stop.png";
	QString playImagePath = assetsDir.absolutePath() + "/play.png";
	QString repeatImagePath = assetsDir.absolutePath() + "/repeat.png";

	_VolumeClickableLabel = new VolumeClickableLabel(speakerImagePath, AudioSlider);
	_VolumeClickableLabel->setFixedSize(screenWidth / 60, screenHeight / 40);
	AudioSlider->setRange(0, 100);
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

	QDir qssDir(currentPath + "/../../TelegramQuickView/src/сss/");
	QString VideoSliderStyle = qssDir.absolutePath() + "/VideoSliderStyle.сss";
	QString AudioSliderStyle = qssDir.absolutePath() + "/AudioSliderStyle.сss";

	QFile VideoSliderStyleFile(VideoSliderStyle);
	QFile AudioSliderStyleFile(AudioSliderStyle);

	if (VideoSliderStyleFile.open(QFile::ReadOnly) && AudioSliderStyleFile.open(QFile::ReadOnly)) {
		VideoSlider->setStyleSheet(VideoSliderStyleFile.readAll());
		AudioSlider->setStyleSheet(AudioSliderStyleFile.readAll());

		VideoSliderStyleFile.close();
		AudioSliderStyleFile.close();
	}

	MediaPlayerWidget->setAudioOutput(audioOutput);
	MediaPlayerWidget->setVideoOutput(GraphicsVideoItem);

	setLayout(gridLayout);

	gridLayout->addWidget(GraphicsView);

	form->setMaximumSize(screenWidth, screenHeight);
	grid->setMaximumSize(screenWidth, screenHeight);

	toolWidget->setMaximumSize(screenWidth, screenHeight);
	toolLayout->setMaximumSize(screenWidth, screenHeight);

	toolWidget->setContentsMargins(0, 0, 0, 0);
	toolLayout->setContentsMargins(0, 0, 0, 0);

	form->setGeometry(QRectF(form->pos().x(), form->pos().y(), size().width(), size().height()));
	grid->setGeometry(QRectF(form->pos().x(), form->pos().y(), size().width(), size().height()));

	toolWidget->setGeometry(QRectF(toolWidget->pos().x(), toolWidget->pos().y(), size().width(), size().height()));
	toolLayout->setGeometry(QRectF(toolWidget->pos().x(), toolWidget->pos().y(), size().width(), size().height()));

	form->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	toolWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	GraphicsAudioSlider = GraphicsScene->addWidget(AudioSlider);
	GraphicsVolumeClickableLabel = GraphicsScene->addWidget(_VolumeClickableLabel);
	GraphicsVideoSlider = GraphicsScene->addWidget(VideoSlider);
	GraphicsVideoTimeLabel = GraphicsScene->addWidget(videoTimeLabel);
	QGraphicsWidget* GraphicsVideoStateWidget = GraphicsScene->addWidget(videoStateWidget);

	QGraphicsWidget* GraphicsMinButton = GraphicsScene->addWidget(minButton);
	QGraphicsWidget* GraphicsMaxButton = GraphicsScene->addWidget(maxButton);
	QGraphicsWidget* GraphicsCloseButton = GraphicsScene->addWidget(closeButton);

	GraphicsAudioSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsVideoSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	GraphicsVideoTimeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsVideoStateWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsMinButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsMaxButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	GraphicsCloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	containerWidget->setLayout(containerLayout);
	toolWidget->setLayout(toolLayout);

	containerLayout->setSpacing(screenWidth / 250);
	containerWidget->setContentsMargins(0, 0, 0, 0);
	grid->setContentsMargins(0, 0, 0, 0);

	containerLayout->addItem(GraphicsVideoSlider, 0, 0, 1, 5, Qt::AlignLeft | Qt::AlignBottom);
	containerLayout->addItem(GraphicsVideoStateWidget, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	containerLayout->addItem(GraphicsVolumeClickableLabel, 1, 1, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	containerLayout->addItem(GraphicsAudioSlider, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	containerLayout->addItem(GraphicsVideoTimeLabel, 1, 3, 1, 1, Qt::AlignLeft | Qt::AlignBottom);


	toolLayout->addItem(GraphicsMinButton, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addItem(GraphicsMaxButton, 0, 1, 1, 1, Qt::AlignRight | Qt::AlignTop);
	toolLayout->addItem(GraphicsCloseButton, 0, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);

	containerLayout->setRowSpacing(1, GraphicsAudioSlider->size().height() * 2);
	toolLayout->setHorizontalSpacing(0);

	containerLayout->setContentsMargins(10, 10, 10, 10);

	GraphicsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	grid->addItem(toolWidget, 0, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	grid->addItem(containerWidget, 1, 0, 1, 1, Qt::AlignBottom);

	form->setLayout(grid);
	GraphicsScene->addItem(form);

	connect(VideoSlider, &QSlider::sliderPressed, [this]() {
		disconnect(MediaPlayerWidget, &QMediaPlayer::positionChanged, VideoSlider, &QSlider::setValue);
		MediaPlayerWidget->pause();
		});

	connect(VideoSlider, &QSlider::sliderReleased, [this]() {
		connect(MediaPlayerWidget, &QMediaPlayer::positionChanged, VideoSlider, &QSlider::setValue);
		_sleep(1);
		MediaPlayerWidget->play();
		});

	connect(MediaPlayerWidget, &QMediaPlayer::durationChanged, VideoSlider, &QSlider::setMaximum);
	connect(MediaPlayerWidget, &QMediaPlayer::positionChanged, this, &MediaPlayer::videoSliderSetValue);
	connect(MediaPlayerWidget, &QMediaPlayer::mediaStatusChanged, this, [this]() {
		if (MediaPlayerWidget->mediaStatus() == MediaPlayerWidget->EndOfMedia) {
			videoStateWidget->stackedWidget()->setCurrentIndex(2);
		}
		});

	connect(VideoSlider, &QSlider::sliderMoved, this, &MediaPlayer::setMediaPlayerVideoPosition);
	connect(AudioSlider, &QSlider::valueChanged, this, &MediaPlayer::volumeChanged);

	connect(_VolumeClickableLabel, &ClickableLabel::clicked, this, [this]() {
		if (AudioSlider->isHidden())
			AudioSlider->show();
		else
			AudioSlider->hide();
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
		MediaPlayerWidget->setPosition(VideoBegin);
		_allowChangeVideoState = true;
		if (MediaPlayerWidget->playbackState() != MediaPlayerWidget->PlayingState) {
			MediaPlayerWidget->play();
		}
		videoStateWidget->stackedWidget()->setCurrentIndex(0);
		});

	QWidgetList widgetsList = QWidgetList({ VideoSlider, _VolumeClickableLabel, videoStateWidget, videoTimeLabel, minButton, maxButton, closeButton });
	WidgetsHider& widgetsHider = WidgetsHider::Instance(widgetsList);
	widgetsHider.SetInactivityDuration(3000);

	connect(&widgetsHider, &WidgetsHider::widgetsShowed, this, &MediaPlayer::adjustBottomWidgets);
	connect(&widgetsHider, &WidgetsHider::widgetsHidden, this, &MediaPlayer::adjustBottomWidgets);

	VideoSlider->hide();
	_VolumeClickableLabel->hide();
	videoStateWidget->hide();
	videoTimeLabel->hide();
	minButton->hide();
	maxButton->hide(); 
	closeButton->hide();

	AudioSlider->setValue(50);
}

void MediaPlayer::volumeChanged(int value)
{
	qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
	audioOutput->setVolume(linearVolume);
}

void MediaPlayer::videoSliderSetValue(int value)
{
	int position = MediaPlayerWidget->position();
	int duration = MediaPlayerWidget->duration();

	int positionSeconds = (position / 1000) % 60;
	int positionMinutes = (position / 1000) / 60;

	videoTimeLabel->setText(QString("%1:%2 / %3:%4")
		.arg(positionMinutes, 2, 10, QChar('0'))
		.arg(positionSeconds, 2, 10, QChar('0'))
		.arg(duration / 60000)
		.arg((duration / 1000) % 60));

	int timeToEnd = duration - position;
	if (timeToEnd <= 1000 && timeToEnd >= 0) {
		MediaPlayerWidget->pause();
		setMediaPlayerVideoPosition(duration-1);
		_allowChangeVideoState = false;
		videoStateWidget->stackedWidget()->setCurrentIndex(2);
		return;
	}

	if (videoStateWidget->stackedWidget()->currentIndex() == 2 && _allowChangeVideoState == false) {
		_allowChangeVideoState = true;
		MediaPlayerWidget->play();
		videoStateWidget->stackedWidget()->setCurrentIndex(0);
	}
	
	VideoSlider->setValue(value);
}

void MediaPlayer::setMediaPlayerVideoPosition(int value)
{
	if (_allowChangeVideoState)
		MediaPlayerWidget->setPosition(value);
}

void MediaPlayer::mouseDoubleClickEvent(QMouseEvent* event)
{
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

void MediaPlayer::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		videoClicked();
	}
	event->accept();
}


void MediaPlayer::videoClicked()
{
	if (!_allowChangeVideoState)
		return;

	if (MediaPlayerWidget->playbackState() == MediaPlayerWidget->PlayingState) {
		MediaPlayerWidget->pause();
	}
	else if (MediaPlayerWidget->playbackState() == MediaPlayerWidget->PausedState) {
		MediaPlayerWidget->play();
	}
}

void MediaPlayer::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);

	form->setGeometry(QRectF(form->pos().x(), form->pos().y(), size().width(), size().height()));
	grid->setGeometry(QRectF(form->pos().x(), form->pos().y(), size().width(), size().height()));

	toolWidget->setGeometry(QRectF(toolWidget->pos().x(), toolWidget->pos().y(), size().width(), size().height()));
	toolLayout->setGeometry(QRectF(toolWidget->pos().x(), toolWidget->pos().y(), size().width(), size().height()));
	if (doubleClicked)
		return;

	adjustVideoSize();

	if (windowState() != Qt::WindowMaximized && windowState() != Qt::WindowFullScreen)
		return;

	isFullScreen() ? showNormal() : showFullScreen();
}

void MediaPlayer::adjustVideoSize()
{
	GraphicsVideoItem->setSize(size());
	GraphicsScene->setSceneRect(QRectF(QPointF(0, 0), size()));
}

void MediaPlayer::adjustBottomWidgets()
{
	if (AudioSlider->isHidden()) {
		containerLayout->removeItem(GraphicsVideoTimeLabel);
		QGraphicsLayoutItem* item = containerLayout->itemAt(1, 2);
		if (item == GraphicsAudioSlider) {
			containerLayout->removeItem(GraphicsAudioSlider);
			containerLayout->addItem(GraphicsAudioSlider, 1, 3, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
		}
		containerLayout->addItem(GraphicsVideoTimeLabel, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	}
	else {
		containerLayout->removeItem(GraphicsVideoTimeLabel);
		QGraphicsLayoutItem* item = containerLayout->itemAt(1, 3);
		if (item == GraphicsAudioSlider) {
			containerLayout->removeItem(GraphicsAudioSlider);
			containerLayout->addItem(GraphicsAudioSlider, 1, 2, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
		}
		containerLayout->addItem(GraphicsVideoTimeLabel, 1, 3, 1, 1, Qt::AlignLeft | Qt::AlignBottom);
	}
}

QString MediaPlayer::detectMediaType(const QString& filePath)
{
	return QMimeDatabase().mimeTypeForFile(filePath).name();
}

void MediaPlayer::setSource(const QUrl& source)
{
	QString sourcePath;
	if (source.path().at(0) == "/"[0])
		sourcePath = source.path().remove(0, 1);
	else
		sourcePath = source.path();

	QString mediaType = detectMediaType(sourcePath);
	if (mediaType.contains("video")) {
		QList<QGraphicsItem*> items = GraphicsScene->items();
		for (QGraphicsItem* item : items) {
			if (qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
				GraphicsScene->removeItem(item);
				delete item;
			}
		}
		containerWidget->show();
		MediaPlayerWidget->setSource(source);
		play();
	}
	else if (mediaType.contains("image")) {
		containerWidget->hide();
		QGraphicsPixmapItem* m_currentImageItem = new QGraphicsPixmapItem(QPixmap(sourcePath));
		GraphicsScene->addItem(m_currentImageItem);
	}
}

void MediaPlayer::play()
{
	MediaPlayerWidget->play();
}
