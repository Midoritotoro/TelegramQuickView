#pragma once

#define EDGE_OF_SCREEN_POSITION -1

#include <Windows.h>
#include <QObject>

#include "../panel/TelegramPostQuickView.h"


class MouseDetector: public QObject {
	Q_OBJECT
public:
	enum class Direction {
		Left,
		Right
	};

	typedef struct _ThreadParameters {
		Direction direction;
		BOOL running;
	} ThreadParameters;

private:
	HANDLE _thread = nullptr;
	ThreadParameters* _lpThreadParameters = nullptr;
Q_SIGNALS:
	void needsToShow();
public:
	MouseDetector();

	void trackMouse();
	bool killThread();

	void setDirection(Direction direction);

	[[nodiscard]] inline Direction getDirection() { return _lpThreadParameters->direction; }
	[[nodiscard]] inline BOOL isRunning() { return _lpThreadParameters->running; }
protected:
	[[nodiscard]] static DWORD WINAPI checkMousePosition(LPVOID lpSelf);
	DWORD WINAPI checkMousePositionMember();
};

