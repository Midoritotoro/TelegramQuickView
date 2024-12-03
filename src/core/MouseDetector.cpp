#include "MouseDetector.h"


MouseDetector::MouseDetector()
{
	_lpThreadParameters = new ThreadParameters();
	_lpThreadParameters->direction = Direction::Right;
}

void MouseDetector::trackMouse() {
	_lpThreadParameters->running = true;
	_thread = CreateThread(nullptr, 0, checkMousePosition, static_cast<LPVOID>(this), 0, nullptr);
}

bool MouseDetector::killThread() {
	_lpThreadParameters->running = false;
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
		const auto isSuccessfullyGetCursorPos = GetCursorPos(&lpCursorPointParameters);
		if (isSuccessfullyGetCursorPos == false)
			continue;

		const auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
		const auto menuThresholdWidthRatio = screenWidth / 64.;

		switch (_lpThreadParameters->direction) {
			case Direction::Right:
				if (screenWidth - lpCursorPointParameters.x <= menuThresholdWidthRatio
					&& (screenWidth - lpCursorPointParameters.x) > EDGE_OF_SCREEN_POSITION)
					emit needsToShow();
				break;

			case Direction::Left:
				if (lpCursorPointParameters.x <= menuThresholdWidthRatio && 
					lpCursorPointParameters.x > EDGE_OF_SCREEN_POSITION)
					emit needsToShow();
				break;
		}
	}
	return 0;
}

DWORD WINAPI MouseDetector::checkMousePosition(LPVOID lpSelf) {
	MouseDetector* self = (MouseDetector*)lpSelf;
	return self->checkMousePositionMember();
}
