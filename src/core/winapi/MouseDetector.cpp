#include "MouseDetector.h"

VOID MouseDetector::TrackMouse(Direction direction) {
	_LPThreadParameters->direction = direction;
	_LPThreadParameters->Running = TRUE;
	_Thread = CreateThread(NULL, 0, d, (LPVOID)this, 0, 0);
}

BOOL MouseDetector::KillThread() {
	FreeConsole();
	_LPThreadParameters->Running = FALSE;
	return CloseHandle(_Thread);
}

DWORD WINAPI MouseDetector::CheckMousePosition() {
	POINT lpCursorPointParameters = { 0 };
	while (_LPThreadParameters->Running)
	{
		BOOL SuccessfullyGetCursorPos = GetCursorPos(&lpCursorPointParameters);
		if (SuccessfullyGetCursorPos == FALSE) {
			continue;
		}
		int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int menuThresholdWidthRatio = ScreenWidth / 64;
		if (_LPThreadParameters->direction == Direction::Right && (ScreenWidth - lpCursorPointParameters.x) <= menuThresholdWidthRatio && (ScreenWidth - lpCursorPointParameters.x) > EDGE_OF_SCREEN_POSITION) {
			PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), TEXT("Мышь находится в правой части экрана \n"));
		}
		else if (_LPThreadParameters->direction == Direction::Left && lpCursorPointParameters.x <= menuThresholdWidthRatio && lpCursorPointParameters.x > EDGE_OF_SCREEN_POSITION) {
			PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), TEXT("Мышь находится в левой части экрана \n"));
		}
		Sleep(200);
	}
	return 0;
}
