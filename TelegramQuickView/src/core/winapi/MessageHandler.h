#pragma once

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#include <windows.h> 
#include <stdarg.h> 
#include <tchar.h>

#include "Exceptions.h"

#define BUF_SIZE 0x200 

BOOL PrintStrings(HANDLE hOut, ...);
BOOL PrintMsg(HANDLE hOut, LPCTSTR pMsg);
VOID ReportError(LPCTSTR UserMessage, DWORD exceptionCode);
VOID printLastError();
