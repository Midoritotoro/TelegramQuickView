#pragma once

#pragma push_macro("slots")
#undef slots
#include <pybind11/pybind11.h>
#pragma pop_macro("slots")

class PYBIND11_EXPORT TelegramAuthorizationChecker {
public:
	TelegramAuthorizationChecker();
	~TelegramAuthorizationChecker();
	[[nodiscard]] bool TelegramCredentialsValidCheck(const char* apiHash, const char* phoneNumber, long long apiId);
	[[nodiscard]] bool sendTelegramCode(const char* apiHash, const char* phoneNumber, long long apiId);
};