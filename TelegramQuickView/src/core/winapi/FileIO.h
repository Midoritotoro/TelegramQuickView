#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>

#include "MessageHandler.h"


bool WriteChannelsToFile(LPCWSTR channels, bool replaceCurrentChannels);
char* ReadChannelsFromFile();
