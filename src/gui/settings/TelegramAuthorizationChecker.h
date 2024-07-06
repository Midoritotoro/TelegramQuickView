#pragma once

#include <iostream>
#include <pybind11/pybind11.h>

class TelegramAuthorizationChecker {
public:
	TelegramAuthorizationChecker();
	~TelegramAuthorizationChecker();
	[[nodiscard]] bool TelegramCredentialsValidCheck(const char* apiHash, const char* phoneNumber, long long apiId);
	[[nodiscard]] bool sendTelegramCode(const char* apiHash, const char* phoneNumber, long long apiId);
};