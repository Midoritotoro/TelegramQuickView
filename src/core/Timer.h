#pragma once

#include <QTimer>
#include "Time.h"


namespace core {

	class Timer final : private QObject {
	public:
		explicit Timer(
			not_null<QThread*> thread,
			std::function<void()> callback = nullptr);
		explicit Timer(std::function<void()> callback = nullptr);

		static Qt::TimerType DefaultType(Time::time timeout) {
			constexpr auto kThreshold = Time::time(240);
			return (timeout > kThreshold) ? Qt::CoarseTimer : Qt::PreciseTimer;
		}

		void setCallback(std::function<void()> callback) {
			_callback = std::move(callback);
		}

		void callOnce(Time::time timeout) {
			callOnce(timeout, DefaultType(timeout));
		}

		void callEach(Time::time timeout) {
			callEach(timeout, DefaultType(timeout));
		}

		void callOnce(Time::time timeout, Qt::TimerType type) {
			start(timeout, type, Repeat::SingleShot);
		}

		void callEach(Time::time timeout, Qt::TimerType type) {
			start(timeout, type, Repeat::Interval);
		}

		bool isActive() const {
			return (_timerId != 0);
		}

		void cancel();
		Time::time remainingTime() const;

		static void Adjust();

	protected:
		void timerEvent(QTimerEvent* e) override;

	private:
		enum class Repeat : unsigned {
			Interval = 0,
			SingleShot = 1,
		};
		void start(Time::time timeout, Qt::TimerType type, Repeat repeat);
		void adjust();

		void setTimeout(Time::time timeout);
		int timeout() const;

		void setRepeat(Repeat repeat) {
			_repeat = static_cast<unsigned>(repeat);
		}

		Repeat repeat() const {
			return static_cast<Repeat>(_repeat);
		}

		std::function<void()> _callback;
		Time::time _next = 0;

		int _timeout = 0;
		int _timerId = 0;

		Qt::TimerType _type : 2;

		bool _adjusted = false;
		unsigned _repeat = 0;

	};


} // namespace core