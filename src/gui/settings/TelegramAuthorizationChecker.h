#pragma once

#define PY_SSIZE_T_CLEAN
#include "../../src/core/PythonQtWrapper.h"

class TelegramAuthorizationChecker {
private:
	PyObject* PyArgs = nullptr, * PySys = nullptr, * PyPath = nullptr, * PyName = nullptr,
		* PyModule = nullptr, * PyDict = nullptr, * PyFunc = nullptr, * PyResult = nullptr;
public:
	TelegramAuthorizationChecker();
	~TelegramAuthorizationChecker();
	[[nodiscard]] bool TelegramCredentialsValidCheck(const char* apiHash, const char* phoneNumber, long long apiId);
	[[nodiscard]] bool sendTelegramCode(const char* apiHash, const char* phoneNumber, long long apiId, const char* pathToUserSettingsJson);
	[[nodiscard]] bool TelegramCodeValidCheck(const char* apiHash, const char* phoneNumber, long long apiId, long code, const char* codeHash);
};