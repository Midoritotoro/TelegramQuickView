#include "MediaPlayer.h"

#include <QByteArray>

#include <QPainter>
#include <QMouseEvent>
#include <QMimeDatabase>

#include <QAudioDevice>
#include <QAudio>
#include <QShortcut>
#include <QPainterPath>

#include <QMetaObject>

#include <QPointF>
#include <QRectF>


namespace {
	inline constexpr auto kPanelBottomIndent = 5;

	[[nodiscard]] QByteArray ReadFile(const QString& filepath) {
		auto file = QFile(filepath);
		return file.open(QIODevice::ReadOnly)
			? file.readAll()
			: QByteArray();
	}

	[[nodiscard]] QImage Prepare(
		QImage image,
		const QSize& _outer)
	{
		const auto imageWidth = image.width();
		const auto imageHeight = image.height();

		if (_outer.width() <= 0 || (_outer.width() == imageWidth
			&& (_outer.height() <= 0 || _outer.height() == imageHeight)))
			;
		else if (imageHeight <= 0)
			image = image.scaledToWidth(
				_outer.width(),
				Qt::SmoothTransformation);
		else
			image = image.scaled(
				_outer.width(),
				_outer.height(),
				Qt::IgnoreAspectRatio,
				Qt::SmoothTransformation);

		auto outer = _outer;

		if (!outer.isEmpty()) {
			const auto ratio = style::DevicePixelRatio();
			outer *= ratio;
			if (outer != QSize(_outer.width(), _outer.height())) {
				image.setDevicePixelRatio(ratio);

				auto result = QImage(outer, QImage::Format_ARGB32_Premultiplied);
				result.setDevicePixelRatio(ratio);

				QPainter painter(&result);

				if (_outer.width() < outer.width() || _outer.height() < outer.height())
					painter.fillRect(
						QRect({}, result.size() / ratio),
						Qt::black);
				painter.drawImage(
					(result.width() - imageWidth) / (2 * ratio),
					(result.height() - imageWidth) / (2 * ratio),
					image);

				image = std::move(result);
			}
		}

		image.setDevicePixelRatio(style::DevicePixelRatio());
		return image;
	}
}


MediaPlayer::MediaPlayer(QWidget* parent) :
	QWidget(parent)
	, _manager(std::make_unique<Manager>())
{
	_mediaPlayerPanel = new MediaPlayerPanel(this);

	resize(1920, 1080);
	setNormal();

	_widgetsHider = std::make_unique<WidgetsHider>(false, true, QWidgetList({ _mediaPlayerPanel }));

	_widgetsHider->SetInactivityDuration(3000);
	_widgetsHider->SetAnimationDuration(3000);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setMouseTracking(true);

	connect(_manager.get(), &Manager::endOfMedia, this, [this] {
		_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Repeat);
		qDebug() << "Media player time taken: " << static_cast<double>(Time::now() - _currMs) / 1000 << " seconds";
		if (_manager->hasVideo() || _manager->hasAudio())
			cleanUp();
		});

	connect(_manager.get(), &Manager::durationChanged, _mediaPlayerPanel, &MediaPlayerPanel::setVideoSliderMaximum);
	connect(_manager.get(), &Manager::positionChanged, this, [this](qint64 position) {
		disconnect(_mediaPlayerPanel->playbackSlider(), &QSlider::valueChanged, _manager.get(), &Manager::rewind);
		_mediaPlayerPanel->playbackSlider()->setValue(position);

		_mediaPlayerPanel->updateTimeText(position, _manager->duration());
		connect(_mediaPlayerPanel->playbackSlider(), &QSlider::valueChanged, _manager.get(), &Manager::rewind);
		});

	connect(_manager.get(), &Manager::playbackStateChanged, this, [this](Manager::State state) {
		_playbackState = state;
		switch (state) {

		case Manager::State::Playing:
			_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Pause);
			break;

		case Manager::State::Paused:
			_mediaPlayerPanel->updateStateWidget(VideoStateWidget::State::Play);
			break;
		}
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoRepeatClicked, [this] {
		rewind(0);
		_currMs = Time::now();
		});

	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoPlayClicked, this, &MediaPlayer::play);
	connect(_mediaPlayerPanel, &MediaPlayerPanel::videoPauseClicked, this, &MediaPlayer::pause);

	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsNormal, this, &MediaPlayer::setNormal);
	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsFullScreen, this, &MediaPlayer::setFullScreen);

	connect(_mediaPlayerPanel->playbackSlider(), &QSlider::valueChanged, _manager.get(), &Manager::rewind);
	connect(_mediaPlayerPanel, &MediaPlayerPanel::mediaPlayerNeedsChangeVolume, this, &MediaPlayer::changeVolume);

	_mediaPlayerPanel->setVolume(20);

	connect(_manager.get(), &Manager::needToRepaint, this, [this](const QImage& image) {
		_current = image;
		update();
		});

	QShortcut* videoStateShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);

	connect(videoStateShortcut, &QShortcut::activated, [this] {
		playbackState() == Manager::State::Playing
			? pause()
			: play();
		});
}

void MediaPlayer::setMedia(const QString& path) {
	if (_manager->hasVideo() || _manager->hasAudio())
		cleanUp();

	_currentMediaType = detectMediaType(path);
	_currentMediaPath = path;

	if (_currentMediaType == MediaType::Unknown)
		return;

	updatePanelVisibility();
	const auto data = ReadFile(path);

	switch (_currentMediaType) {
	case MediaType::Video:
		_manager->setVideo(std::move(std::make_unique<FFmpeg::FrameGenerator>(data)));
		play();

		_currMs = Time::now();
		break;
	case MediaType::Image:
		_current.loadFromData(data);
		_current = prepareImage(_current);

		update();
		break;

	case MediaType::Audio:
		auto audioReader = std::make_unique<FFmpeg::AudioReader>(data);

		_manager->setAudio(std::move(audioReader));
		play();

		_currMs = Time::now();
		break;
	}
}

int MediaPlayer::getVideoControlsHeight() const noexcept {
	return	!_mediaPlayerPanel->isHidden()
		? _mediaPlayerPanel->height()
		: 0;
}

MediaPlayer::MediaType MediaPlayer::detectMediaType(const QString& filePath) {
	const auto mimeType = QMimeDatabase().mimeTypeForFile(filePath).name();

	if (mimeType.contains("video"))
		return MediaType::Video;
	else if (mimeType.contains("image"))
		return MediaType::Image;
	else if (mimeType.contains("audio"))
		return MediaType::Audio;

	return MediaType::Unknown;
}

QSize MediaPlayer::occupiedMediaSpace() const noexcept {
	return _currentFrameRect.size();
}

QPoint MediaPlayer::mediaPosition() const noexcept {
	return QPoint(_currentFrameRect.x(), _currentFrameRect.y());
}

Manager::State MediaPlayer::playbackState() const noexcept {
	return _playbackState;
}

void MediaPlayer::play() {
	if (playbackState() != Manager::State::Stopped)
		QMetaObject::invokeMethod(_manager.get(), "play", Qt::QueuedConnection);
}

void MediaPlayer::pause() {
	QMetaObject::invokeMethod(_manager.get(), "pause", Qt::QueuedConnection);
}

void MediaPlayer::rewind(Time::time positionMs) {
	if (_manager->hasVideo() == false && _currentMediaPath.isEmpty() == false)
		setMedia(_currentMediaPath);
	QMetaObject::invokeMethod(_manager.get(), "rewind", Qt::QueuedConnection, Q_ARG(Time::time, positionMs));
}

void MediaPlayer::cleanUp() {
	QMetaObject::invokeMethod(_manager.get(), "cleanUp", Qt::QueuedConnection);
}

void MediaPlayer::resizeEvent(QResizeEvent* event) {
	_mediaPlayerPanel->move((width() - _mediaPlayerPanel->width()) / 2,
		height() - _mediaPlayerPanel->height() - kPanelBottomIndent);
}

void MediaPlayer::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	paintBackground(painter);

	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	const auto center = _current.size().width() < size().width()
		|| _current.size().height() < size().height()
		? QPoint((width() - _current.width()) / 2, (height() - _current.height()) / 2) : QPoint(0, 0);

	_currentFrameRect = QRect(center, _current.size());
	painter.drawImage(center, _current);
}

void MediaPlayer::mousePressEvent(QMouseEvent* event) {
	if (!(event->button() == Qt::LeftButton && _currentFrameRect.contains(event->pos())))
		return;

	switch (playbackState()) {

	case Manager::State::Playing:
		pause();
		break;

	case Manager::State::Paused:
		if (_manager->hasVideo() == false && _currentMediaPath.isEmpty() == false)
			setMedia(_currentMediaPath);

		if ((_manager->duration() - _manager->position()) <= 100)
			rewind(0);
		play();
		break;
	}
}


void MediaPlayer::updatePanelVisibility() {
	switch (_currentMediaType) {
	case MediaType::Video:
		_widgetsHider->addWidget(_mediaPlayerPanel);
		_widgetsHider->resetTimer();

		_mediaPlayerPanel->show();
		break;

	case MediaType::Image:
		_widgetsHider->removeWidget(_mediaPlayerPanel);
		_widgetsHider->resetTimer();

		_mediaPlayerPanel->hide();
		break;

	case MediaType::Audio:
		break;
	}
}

void MediaPlayer::setFullScreen() {
	_displayType = MediaDisplayType::FullScreen;
	_manager->setDisplayType(false);

	_manager->setTargetSize(size());
	emit mediaGeometryChanged();

	update();
}

void MediaPlayer::setNormal() {
	_displayType = MediaDisplayType::Normal;
	_manager->setDisplayType(true);

	_manager->setTargetSize(QSize());
	emit mediaGeometryChanged();

	update();
}

QImage MediaPlayer::prepareImage(const QImage& sourceImage) {
	const auto ms = Time::now();
	const auto timeCheck = Guard::finally([&ms] { qDebug() << "MediaPlayer::prepareImage: " << Time::now() - ms << " ms"; });

	auto resolveSize = [=](const QSize& size) -> QSize {
		const auto screenSize = QApplication::primaryScreen()->availableGeometry().size();

		double scale = qMin(static_cast<double>(screenSize.width()) / size.width(),
			static_cast<double>(screenSize.height()) / size.height());

		if (size.width() * scale <= (screenSize.width() * 0.7))
			return size;

		scale = qMin(scale, (screenSize.width() * 0.7) / size.width());

		return QSize(size.width() * scale, size.height() * scale);
		};

	return Prepare(sourceImage, resolveSize(sourceImage.size()));
}

void MediaPlayer::changeVolume(int value) {
	const auto linearVolume = QAudio::convertVolume(value / qreal(100),
		QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
}

void MediaPlayer::paintBackground(QPainter& painter) {
	const auto opacity = painter.opacity();

	painter.setOpacity(0.5);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::black);

	painter.drawRect(rect());

	painter.setOpacity(1.0);
}