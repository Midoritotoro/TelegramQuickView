#pragma once

#define PY_SSIZE_T_CLEAN
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

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