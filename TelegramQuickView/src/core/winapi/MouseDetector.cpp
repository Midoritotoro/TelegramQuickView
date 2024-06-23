#include "MouseDetector.h"

VOID MouseDetector::TrackMouse(Direction direction)
{
	_LPThreadParameters->direction = direction;
	_LPThreadParameters->Running = TRUE;
	_Thread = CreateThread(NULL, 0, CheckMousePosition, _LPThreadParameters, 0, 0);
}

BOOL MouseDetector::KillThread()
{
	FreeConsole();
	_LPThreadParameters->Running = FALSE;
	return CloseHandle(_Thread);
}

DWORD WINAPI MouseDetector::CheckMousePosition(LPVOID lpParam)
{
	AllocConsole();
	LPThreadParameters lpThreadParameters = static_cast<LPThreadParameters>(lpParam);
	POINT lpCursorPointParameters = { 0 };
	while (lpThreadParameters->Running)
	{
		BOOL SuccessfullyGetCursorPos = GetCursorPos(&lpCursorPointParameters);
		if (SuccessfullyGetCursorPos == FALSE) {
			continue;
		}
		int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int menuThresholdWidthRatio = ScreenWidth / 64;
		if (lpThreadParameters->direction == Direction::Right && (ScreenWidth - lpCursorPointParameters.x) <= menuThresholdWidthRatio && (ScreenWidth - lpCursorPointParameters.x) > EDGE_OF_SCREEN_POSITION) {
			PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), L"Мышь находится в правой части экрана \n");
		}
		else if (lpThreadParameters->direction == Direction::Left && lpCursorPointParameters.x <= menuThresholdWidthRatio && lpCursorPointParameters.x > EDGE_OF_SCREEN_POSITION) {
			PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), L"Мышь находится в левой части экрана \n");
		}
		Sleep(200);
	}
	return 0;
}
