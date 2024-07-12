#pragma once

#define PY_SSIZE_T_CLEAN
#include "../../src/core/PythonQtWrapper.h"

#include <Windows.h>
#include <string>


typedef struct _TelegramCredentials {
	std::string apiHash;
	std::string phoneNumber;
	std::string codeHash;
	std::string pathToUserSettingsJson;
	long long apiId;
	long code;
} TelegramCredentials, *LPTelegramCredentials;


class TelegramAuthorizationChecker {
public:
	[[nodiscard]] static DWORD WINAPI TelegramCredentialsValidCheck(LPVOID lpParam);
	[[nodiscard]] static DWORD WINAPI sendTelegramCode(LPVOID lpParam);
	[[nodiscard]] static DWORD WINAPI TelegramCodeValidCheck(LPVOID lpParam);
};