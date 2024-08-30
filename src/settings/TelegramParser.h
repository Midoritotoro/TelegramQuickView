#pragma once

#include <string>

#include <td/telegram/td_json_client.h> 
#include <td/telegram/td_api.h> // Version 1.8.35


typedef struct _TelegramAuthorizationInfo {
	std::string apiHash;
	std::string apiId;
	std::string phoneNumber;
} TelegramAuthorizationInfo;


class TelegramParser {
private:
	void* _tdJsonClient = nullptr;
	int _tdClientId;

	TelegramAuthorizationInfo _telegramAuthorizationInfo;
public:
	TelegramParser(const TelegramAuthorizationInfo& telegramAuthorizationInfo);
	~TelegramParser();

	void checkLastChannelUpdates();
private:
	[[nodiscard]] bool authorizeToAccount();
	[[nodiscard]] bool isUserAuthorized();
};