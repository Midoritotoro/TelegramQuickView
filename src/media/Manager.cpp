#include "Manager.h"

#include <QDebug>
#include <QAbstractEventDispatcher>

#include <functional>

Manager::Manager():
	_state(State::Stopped)
{
	_thread = new QThread;

	connect(_thread, &QThread::started, this, [=] { process(); });

	_timer.setSingleShot(true);
	_timer.moveToThread(_thread);

	connect(&_timer, &QTimer::timeout, this, [=] { process(); });

	moveToThread(_thread);

	emit playbackStateChanged(_state);
}

Manager::~Manager() {
	_thread->quit();
	cleanUp();
}

void Manager::cleanUp() {
	QMutexLocker locker(&_mutex);

	_state = State::Stopped;
	emit playbackStateChanged(_state);

	if (hasVideo()) {
		_frameGenerator.reset();
		_frameGenerator = nullptr;
	}

	if (hasAudio()) {
		_audioReader.reset();
		_audioReader = nullptr;
	}


	if (_thread->isRunning())
		_thread->quit();

	_state = State::Playing;
}

Time::time Manager::duration() const noexcept {
	return hasVideo() 
		? _frameGenerator->duration()
		: 0;
}

Time::time Manager::position() const noexcept {
	return hasVideo()
		? _frameGenerator->position()
		: 0;
}

void Manager::pause() {
	_state = State::Paused;
	emit playbackStateChanged(State::Paused);
}

void Manager::play() {
	_state = State::Playing;
	emit playbackStateChanged(State::Playing);

	_timer.start(1);
}

void Manager::process() {
	if (_state != State::Playing) 
		return;

	if (hasVideo()) {
		const auto ms = Time::now();
		const auto frame = _frameGenerator->renderNext(_size, Qt::IgnoreAspectRatio, _showNormal);

		if (frame.last)
			return emit endOfMedia();

		emit positionChanged(_frameGenerator->position());
		emit needToRepaint(frame.image);

		auto nowMs = Time::now() - ms;
		const auto timeout = qMax(1, _frameGenerator->frameDelay() - nowMs);

		qDebug() << "_frameGenerator->frameDelay(): " << _frameGenerator->frameDelay();
		qDebug() << timeout;

		_timer.start(timeout);
	}
}

void Manager::setTargetSize(const QSize& size) {
	QMutexLocker locker(&_mutex);
	_size = size;
}

void Manager::setDisplayType(bool showNormal) {
	QMutexLocker locker(&_mutex);
	_showNormal = showNormal;
}

void Manager::setVideo(std::unique_ptr<FFmpeg::FrameGenerator>&& generator, const QSize& size) {
	QMutexLocker locker(&_mutex);

	_size = size;
	_frameGenerator = std::move(generator);

	emit durationChanged(_frameGenerator->duration());

	if (!_thread->isRunning())
		_thread->start();
	_thread->setPriority(QThread::HighestPriority);
}

void Manager::setAudio(std::unique_ptr<FFmpeg::AudioReader>&& audio) {
	QMutexLocker locker(&_mutex);

	_audioReader = std::move(audio);
	emit durationChanged(_audioReader->duration());

	if (!_thread->isRunning())
		_thread->start();

	_thread->setPriority(QThread::HighestPriority);
}

bool Manager::hasVideo() const noexcept {
	return _frameGenerator != nullptr;
}


bool Manager::hasAudio() const noexcept {
	return _audioReader != nullptr;
}

void Manager::rewind(Time::time position) {
	QMutexLocker locker(&_mutex);

	qDebug() << "Rewind: " << position;

	emit positionChanged(position);

	if (hasVideo()) {
		_frameGenerator->rewind(position);

		const auto frame = _frameGenerator->renderNext(_size, Qt::IgnoreAspectRatio, _showNormal);
		emit needToRepaint(frame.image);

		if (_frameGenerator->position() >= _frameGenerator->duration() - _frameGenerator->frameDelay())
			emit endOfMedia();
		else if (_state == State::Playing)
			play();
	}

	/*if (hasAudio()) {
		_audioReader->open(position);

		if (_audioReader->position() >= _audioReader->duration())
			emit endOfMedia();
		else if (_state == State::Playing)
			play();
	}*/
}