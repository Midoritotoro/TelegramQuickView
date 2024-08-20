#include "MouseDetector.h"


MouseDetector::MouseDetector()
{
	_lpThreadParameters = new ThreadParameters();
	_lpThreadParameters->direction = Direction::Right;
}

void MouseDetector::trackMouse() {
	_lpThreadParameters->running = TRUE;
#ifdef _WIN32	
	_thread = CreateThread(NULL, 0, checkMousePosition, (LPVOID)this, 0, 0);
#endif // _WIN32
}

bool MouseDetector::killThread() {
	_lpThreadParameters->running = FALSE;
	return CloseHandle(_thread);
}

void MouseDetector::setDirection(Direction direction) {
	_lpThreadParameters->direction = direction;
}

DWORD WINAPI MouseDetector::checkMousePositionMember() {
	POINT lpCursorPointParameters = { 0 };

	while (_lpThreadParameters->running)
	{
		Sleep(10);
		BOOL isSuccessfullyGetCursorPos = GetCursorPos(&lpCursorPointParameters);
		if (isSuccessfullyGetCursorPos == FALSE)
			continue;

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int menuThresholdWidthRatio = screenWidth / 64;

		if (_lpThreadParameters->direction == Direction::Right && (screenWidth - lpCursorPointParameters.x) <= menuThresholdWidthRatio && (screenWidth - lpCursorPointParameters.x) > EDGE_OF_SCREEN_POSITION) {
			;
		}
		else if (_lpThreadParameters->direction == Direction::Left && lpCursorPointParameters.x <= menuThresholdWidthRatio && lpCursorPointParameters.x > EDGE_OF_SCREEN_POSITION) {
			;
		}
	}
	return 0;
}

DWORD WINAPI MouseDetector::checkMousePosition(LPVOID lpSelf) {
	MouseDetector* self = (MouseDetector*)lpSelf;
	return self->checkMousePositionMember();
}
