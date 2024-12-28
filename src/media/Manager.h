#pragma once


#include "ffmpeg/video/FrameGenerator.h"
#include "ffmpeg/audio/AudioReader.h"

#include <QThread>
#include <QTimer>
#include <QMutex>

#include <memory>


class Manager: public QObject {
	Q_OBJECT
public:
	enum class State {
		Paused,
		Playing,
		Stopped
	};
Q_SIGNALS:
	void endOfMedia();
	void needToRepaint(const QImage& frame, int timeout);

	void positionChanged(qint64 position);
	void durationChanged(qint64 duration);

	void playbackStateChanged(State state);
public: 
	Manager();
	~Manager();

	Q_INVOKABLE void cleanUp();

	[[nodiscard]] Time::time duration() const noexcept;
	[[nodiscard]] Time::time position() const noexcept;

	Q_INVOKABLE void pause();
	Q_INVOKABLE void play();

	Q_INVOKABLE void setTargetSize(const QSize& size);
	Q_INVOKABLE void setDisplayType(bool showNormal);

	void setVideo(std::unique_ptr<FFmpeg::FrameGenerator>&& generator, const QSize& size = QSize());
	void setAudio(std::unique_ptr<FFmpeg::AudioReader>&& audio);

	[[nodiscard]] bool hasVideo() const noexcept;
	[[nodiscard]] bool hasAudio() const noexcept;

	Q_INVOKABLE void rewind(Time::time position);
	Q_INVOKABLE void setSpeed(float speed);
private:
	void process();

	QTimer _timer;

	QThread* _thread = nullptr;
	QMutex _mutex;

	QSize _size;

	std::unique_ptr<FFmpeg::FrameGenerator> _frameGenerator = nullptr;
	std::unique_ptr<FFmpeg::AudioReader> _audioReader = nullptr;

	State _state;

	bool _showNormal = false;
};