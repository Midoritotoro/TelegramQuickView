#pragma once

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#include <windows.h>
#include <tchar.h>

#include "MessageHandler.h"

#define EDGE_OF_SCREEN_POSITION -1

class MouseDetector
{	
public:
	typedef enum _Direction {
		Left,
		Right
	} Direction;

	typedef struct _ThreadParameters {
		Direction direction;
		BOOL Running;
	} ThreadParameters, *LPThreadParameters;
private:
	HANDLE _Thread = nullptr;
	LPThreadParameters _LPThreadParameters = new ThreadParameters();
public:
	VOID TrackMouse(Direction direction);
	BOOL KillThread();
	Direction getDirection() { return _LPThreadParameters->direction; }
	BOOL isRunning() { return _LPThreadParameters->Running; }
	static DWORD WINAPI CheckMousePosition(LPVOID lpParam);
};

