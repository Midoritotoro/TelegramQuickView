#pragma once

#define PY_SSIZE_T_CLEAN
#include "../../src/core/PythonQtWrapper.h"

#include <Windows.h>

typedef struct _TelegramCredentials {
	const char* apiHash;
	const char* phoneNumber;
	long long apiId;
	const char* pathToUserSettingsJson;
	long code;
	const char* codeHash;
} TelegramCredentials, *LPTelegramCredentials;

class TelegramAuthorizationChecker {
public:
	[[nodiscard]] static DWORD WINAPI TelegramCredentialsValidCheck(LPVOID lpParam);
	[[nodiscard]] static DWORD WINAPI sendTelegramCode(LPVOID lpParam);
	[[nodiscard]] static DWORD WINAPI TelegramCodeValidCheck(LPVOID lpParam);
};