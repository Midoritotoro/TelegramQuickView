#include "PythonCaller.h"

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#include <Windows.h>
#include <tchar.h>

int main() {
	TCHAR pathToCurrentFile[MAX_PATH];
	const char* PyModuleName = "TelegramRobber";
	const char* TelegramUsername = "antifishechki";
	const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
	const char* phoneNumber = "+375292384917";
	const char* downloadPath = "D:\\Media";
	long long apiId = 13711370;
	//_wgetcwd(pathToCurrentFile, MAX_PATH);
	//DWORD count;
	//WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), pathToCurrentFile, _tcslen(pathToCurrentFile), &count, NULL);
	PythonCaller pythonCaller(apiHash, phoneNumber, downloadPath, apiId, "D:\\TelegramQuickView\\src\\core\\parser\\");
	pythonCaller.CallTelegramParseFunction(PyModuleName, TelegramUsername, "2024-05-04", "2024-05-05");
	return 0;
}