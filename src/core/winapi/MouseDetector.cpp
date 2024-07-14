﻿#include "MouseDetector.h"

VOID MouseDetector::TrackMouse(Direction direction) {
	_lpThreadParameters->direction = direction;
	_lpThreadParameters->Running = TRUE;
	_thread = CreateThread(NULL, 0, CheckMousePosition, (LPVOID)this, 0, 0);
}

BOOL MouseDetector::KillThread() {
	_lpThreadParameters->Running = FALSE;
	return CloseHandle(_thread);
}

DWORD WINAPI MouseDetector::CheckMousePositionMember() {
	POINT lpCursorPointParameters = { 0 };

	while (_lpThreadParameters->Running)
	{
		BOOL isSuccessfullyGetCursorPos = GetCursorPos(&lpCursorPointParameters);
		if (isSuccessfullyGetCursorPos == FALSE)
			continue;

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int menuThresholdWidthRatio = screenWidth / 64;

		if (_lpThreadParameters->direction == Direction::Right && (screenWidth - lpCursorPointParameters.x) <= menuThresholdWidthRatio && (screenWidth - lpCursorPointParameters.x) > EDGE_OF_SCREEN_POSITION) {
			PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), TEXT("Мышь находится в правой части экрана \n"));
		}
		else if (_lpThreadParameters->direction == Direction::Left && lpCursorPointParameters.x <= menuThresholdWidthRatio && lpCursorPointParameters.x > EDGE_OF_SCREEN_POSITION) {
			PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), TEXT("Мышь находится в левой части экрана \n"));
		}
		Sleep(200);
	}
	return 0;
}

DWORD WINAPI MouseDetector::CheckMousePosition(LPVOID lpSelf) {
	MouseDetector* self = (MouseDetector*)lpSelf;
	return self->CheckMousePositionMember();
}
