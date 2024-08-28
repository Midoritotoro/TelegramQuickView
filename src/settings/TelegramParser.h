#pragma once

#include <iostream>
#include <string>

#ifdef _WIN32
	#include <Windows.h>
#endif // _WIN32

#include <nlohmann/json.hpp>

#include <td/telegram/td_json_client.h> 
#include <td/telegram/td_api.h> // Version 1.8.35


class TelegramParser {
private:
	void* _tdJsonClient;
	int _tdClientId;
public:
	TelegramParser(std::string apiHash, std::string apiId, std::string phoneNumber);
	~TelegramParser();
private:
	void authorizeToAccount(std::string apiHash, std::string apiId, std::string phoneNumber);
};