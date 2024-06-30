#include "MessageHandler.h"

BOOL PrintStrings(HANDLE hOut, ...)
{
	DWORD MsgLen, Count;
	LPCTSTR pMsg;
	va_list pMsgList;
	va_start(pMsgList, hOut);
	while ((pMsg = va_arg(pMsgList, LPCTSTR)) != NULL) {
		MsgLen = _tcslen(pMsg);
		if (!WriteConsole(hOut, pMsg, MsgLen, &Count, NULL) && !WriteFile(hOut, pMsg, MsgLen * sizeof(TCHAR), &Count, NULL))
			return FALSE;
	}
	va_end(pMsgList);
	return TRUE;
}

BOOL PrintMsg(HANDLE hOut, LPCTSTR pMsg)
{
	return PrintStrings(hOut, pMsg, NULL);
}

VOID ReportError(LPCTSTR UserMessage, DWORD exceptionCode)
{
	DWORD eMsgLen, LastErr = GetLastError();
	LPTSTR lpvSysMsg;
	HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
	PrintMsg(hStdErr, UserMessage);
	eMsgLen = FormatMessage
	(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, LastErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpvSysMsg, 0, NULL);
	PrintStrings(hStdErr, _T("\n"), lpvSysMsg,
		_T("\n"), NULL);
	HeapFree(GetProcessHeap(), 0, lpvSysMsg);
	if (exceptionCode & VIEWER_ERROR)
		ExitProcess(exceptionCode);
}

VOID printLastError()
{
	DWORD Count = NULL;
	LPSTR messageBuffer = NULL;
	DWORD errorMessageID = GetLastError();
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	int wideCharCount = MultiByteToWideChar(CP_ACP, 0, messageBuffer, -1, NULL, 0);
	WCHAR* wideCharBuffer = new wchar_t[wideCharCount];
	MultiByteToWideChar(CP_ACP, 0, messageBuffer, -1, wideCharBuffer, wideCharCount);

	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), wideCharBuffer, wideCharCount, &Count, NULL);
	delete[] wideCharBuffer;
	LocalFree(messageBuffer);
}