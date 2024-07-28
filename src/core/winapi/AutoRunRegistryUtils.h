#pragma once

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#include <windows.h>
#include <tchar.h>

#include <pathcch.h>
#pragma comment(lib, "Pathcch.lib")

#define MINIMUM_WINDOWS_VERSION NTDDI_WIN10

[[nodiscard]] bool IsWindowsGreaterThen(int version);
[[nodiscard]] BOOL addParserToRegistryAutoRun();
[[nodiscard]] BOOL SetRegistryAutoRunKey(LPWSTR path);

#define IS_MINIMUM_WINDOWS_VERSION IsWindowsGreaterThen(MINIMUM_WINDOWS_VERSION)
