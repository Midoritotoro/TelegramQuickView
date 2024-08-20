#pragma once

#define EDGE_OF_SCREEN_POSITION -1

#ifdef _WIN32
	#include <Windows.h>
#endif // _WIN32

#include "../panel/TelegramPostQuickView.h"


class MouseDetector
{	
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
public:
	MouseDetector();

	void trackMouse();
	bool killThread();

	void setDirection(Direction direction);

	[[nodiscard]] inline Direction getDirection() { return _lpThreadParameters->direction; }
	[[nodiscard]] inline BOOL isRunning() { return _lpThreadParameters->running; }
	
	[[nodiscard]] static DWORD WINAPI checkMousePosition(LPVOID lpSelf);
protected:
	DWORD WINAPI checkMousePositionMember();
};

