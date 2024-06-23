#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <pathcch.h>
#pragma comment(lib, "Pathcch.lib")

#include "Exceptions.h"
#include "MessageHandler.h"



#define MINIMUM_WINDOWS_VERSION NTDDI_WIN10

bool IsWindowsGreaterThen(int version);
BOOL addParserToRegistryAutoRun();
LPWSTR GetPathToCurrentFile(char* path);
BOOL SetRegistryAutoRunKey(LPWSTR path);

#define IS_MINIMUM_WINDOWS_VERSION IsWindowsGreaterThen(MINIMUM_WINDOWS_VERSION)
