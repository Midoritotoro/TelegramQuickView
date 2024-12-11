#include "Timer.h"


namespace core {
	Timer::Timer(
		QThread* thread,
		Fn<void()> callback)
		: Timer(std::move(callback)) {
		moveToThread(thread);
	}

	Timer::Timer(Fn<void()> callback)
		: QObject(nullptr)
		, _callback(std::move(callback))
		, _type(Qt::PreciseTimer) {
		setRepeat(Repeat::Interval);
		connect(
			TimersAdjuster(),
			&QObject::destroyed,
			this,
			[this] { adjust(); },
			Qt::QueuedConnection);
	}

	void Timer::start(Time::time timeout, Qt::TimerType type, Repeat repeat) {
		cancel();

		_type = type;
		setRepeat(repeat);
		_adjusted = false;
		setTimeout(timeout);
		_timerId = startTimer(_timeout, _type);
		if (_timerId) {
			_next = Time::now() + _timeout;
		}
		else {
			_next = 0;
		}
	}

	void Timer::cancel() {
		if (isActive()) {
			killTimer(std::exchange(_timerId, {} ));
		}
	}

	Time::time Timer::remainingTime() const {
		if (!isActive()) {
			return -1;
		}
		const auto now = Time::now();
		return (_next > now) ? (_next - now) : Time::time(0);
	}

	void Timer::Adjust() {
		QObject emitter;
		connect(
			&emitter,
			&QObject::destroyed,
			TimersAdjuster(),
			&QObject::destroyed);
	}

	void Timer::adjust() {
		auto remaining = remainingTime();
		if (remaining >= 0) {
			cancel();
			_timerId = startTimer(remaining, _type);
			_adjusted = true;
		}
	}

	void Timer::setTimeout(Time::time timeout) {
		if (timeout <= 0 || timeout >= std::numeric_limits<int>::max())
			return;

		_timeout = static_cast<unsigned int>(timeout);
	}

	int Timer::timeout() const {
		return _timeout;
	}

	void Timer::timerEvent(QTimerEvent* e) {
		if (repeat() == Repeat::Interval) {
			if (_adjusted) {
				start(_timeout, _type, repeat());
			}
			else {
				_next = Time::now() + _timeout;
			}
		}
		else {
			cancel();
		}

		if (const auto onstack = _callback) {
			onstack();
		}
	}
} // namespace core 