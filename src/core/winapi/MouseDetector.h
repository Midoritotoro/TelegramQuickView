#pragma once

#define EDGE_OF_SCREEN_POSITION -1

#include <windows.h>
#include <tchar.h>

class MouseDetector
{	
public:
	typedef enum _Direction {
		Left,
		Right
	} Direction;

	typedef struct _ThreadParameters {
		Direction direction;
		BOOL running;
	} ThreadParameters, *LPThreadParameters;

private:
	HANDLE _thread = nullptr;
	LPThreadParameters _lpThreadParameters = new ThreadParameters();
public:
	VOID TrackMouse(Direction direction);
	BOOL KillThread();
	[[nodiscard]] Direction getDirection() { return _lpThreadParameters->direction; }
	[[nodiscard]] BOOL isRunning() { return _lpThreadParameters->running; }
	[[nodiscard]] static DWORD WINAPI CheckMousePosition(LPVOID lpSelf);
protected:
	DWORD WINAPI CheckMousePositionMember();
};

