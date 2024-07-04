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
	[[nodiscard]] bool callTelegramAuthorizeCheck(const char* apiHash, const char* phoneNumber, long long apiId);
};