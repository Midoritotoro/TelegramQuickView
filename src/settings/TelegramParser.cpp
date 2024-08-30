#include "TelegramParser.h"

#include <iostream>

#ifdef _WIN32
	#include <Windows.h>
#endif // _WIN32

#include <nlohmann/json.hpp>

#define TELEGRAM_CLIENT_WAIT_TIMEOUT 10


TelegramParser::TelegramParser(const TelegramAuthorizationInfo& telegramAuthorizationInfo):
	_telegramAuthorizationInfo(telegramAuthorizationInfo)
{
	td_execute("{\"@type\":\"setLogVerbosityLevel\", \"new_verbosity_level\":0}");

	/*_telegramAuthorizationInfo.phoneNumber.erase(std::remove_if(_telegramAuthorizationInfo.phoneNumber.begin(),
		_telegramAuthorizationInfo.phoneNumber.end(), std::isspace), _telegramAuthorizationInfo.phoneNumber.end());
	if (_telegramAuthorizationInfo.phoneNumber.at(0) != '+')
		_telegramAuthorizationInfo.phoneNumber = "+" + _telegramAuthorizationInfo.phoneNumber;*/

	_tdClientId = td_create_client_id();
	_tdJsonClient = td_json_client_create();
}

TelegramParser::~TelegramParser() {
	td_json_client_destroy(_tdJsonClient);
}

void TelegramParser::checkLastChannelUpdates() {
	if (!isUserAuthorized())
		if (!authorizeToAccount())
			return;

	std::cout << "aaauth" << std::endl;

	while (true) {
		td_json_client_send(_tdJsonClient, "{ \"@type\": \"getAuthorizationState\", \"@extra\": \"rerfsdfesf\" }");
		const char* received = td_json_client_receive(_tdJsonClient, TELEGRAM_CLIENT_WAIT_TIMEOUT);

		if (!received)
			continue;

		std::cout << received << std::endl;
		if (std::string(received).find("rerfsdfesf") != std::string::npos)
			return;
	}
}

bool TelegramParser::authorizeToAccount() {
	nlohmann::json jsonParsed;
	std::string authorizationStatus, extraParameter, code;

	std::string payloadSetTdlibParameters = "{ \"@type\": \"setTdlibParameters\","
		" \"api_id\": " + _telegramAuthorizationInfo.apiId + ", \"api_hash\": \"" + _telegramAuthorizationInfo.apiHash + "\"," +
		"\"use_test_dc\": false, \"device_model\": \"Desktop\", \"system_version\": \"Unknown\", \"application_version\": \"0.0\", \"system_language_code\": \"en\","
		"\"database_directory\": \"\", \"files_directory\": \"\", \"use_file_database\": true, \"use_chat_info_database\": true,"
		"\"use_message_database\": true, \"use_secret_chats\": false, \"enable_storage_optimizer\": true, \"ignore_file_names\": false, \"@extra\": \"authorizeToAccount\"}";

	std::string payloadSetAuthenticationPhoneNumber = "{ \"@type\": \"setAuthenticationPhoneNumber\", \"phone_number\": \"" + _telegramAuthorizationInfo.phoneNumber + "\", \"@extra\": \"authorizeToAccount\" }";

	while (true) {
		td_json_client_send(_tdJsonClient, "{ \"@type\": \"getAuthorizationState\", \"@extra\": \"authorizeToAccount\" }");
		const char* received = td_json_client_receive(_tdJsonClient, TELEGRAM_CLIENT_WAIT_TIMEOUT);

		if (received == NULL || std::string(received).find("error") != std::string::npos)
			continue;

		jsonParsed = nlohmann::json::parse(received);

		if (jsonParsed.dump().find("@type") == std::string::npos
			|| jsonParsed.dump().find("@extra") == std::string::npos)
			continue;

		authorizationStatus = jsonParsed.at("@type").dump();
		extraParameter = jsonParsed.at("@extra").dump();

		if (authorizationStatus.find("authorizationStateClosed") != std::string::npos
			&& extraParameter.find("authorizeToAccount") != std::string::npos) {
			// Клиент закрыт
			return false;
		}
		else if (authorizationStatus.find("authorizationStateWaitTdlibParameters") != std::string::npos
			&& extraParameter.find("authorizeToAccount") != std::string::npos) {
			// Необходимо ввести параметры TdLib
			td_json_client_send(_tdJsonClient, payloadSetTdlibParameters.c_str());
		}
		else if (authorizationStatus.find("authorizationStateWaitPhoneNumber") != std::string::npos
			&& extraParameter.find("authorizeToAccount") != std::string::npos) {
			// Необходимо ввести телефонный номер пользователя
			td_json_client_send(_tdJsonClient, payloadSetAuthenticationPhoneNumber.c_str());
		}
		else if (authorizationStatus.find("authorizationStateWaitCode") != std::string::npos
			&& extraParameter.find("authorizeToAccount") != std::string::npos) {
			// Пользователю необходимо ввести код для входа в Телеграм
		/*	std::string payloadSendAuthenticationCode = "{ \"@type\": \"sendPhoneNumberCode\", \"phone_number\": \"" + _telegramAuthorizationInfo.phoneNumber + "\"@extra\": \"authorizeToAccount\" }";
			td_json_client_send(_tdJsonClient, payloadSendAuthenticationCode.c_str());*/

			std::cout << "Введите код для входа в Телеграм: " << std::endl;
			std::cin >> code;

			std::string payloadCheckAuthenticationCode = "{ \"@type\": \"checkAuthenticationCode\", \"code_\": \"" + code + "\" }";

			td_json_client_send(_tdJsonClient, payloadCheckAuthenticationCode.c_str());
		}
		else if (authorizationStatus.find("authorizationStateReady") != std::string::npos
			&& extraParameter.find("authorizeToAccount") != std::string::npos) {
			// Вход выполнен успешно
			return true;
		}

		std::cout << authorizationStatus << std::endl;
	}
}

bool TelegramParser::isUserAuthorized() {
	nlohmann::json parsedJson;
	std::string extraParameter;

	td_json_client_send(_tdJsonClient, "{ \"@type\": \"getAuthorizationState\", \"@extra\": \"checkLastChannelUpdates AuthorizationStateCheck\" }");

	while (true) {
		const char* received = td_json_client_receive(_tdJsonClient, TELEGRAM_CLIENT_WAIT_TIMEOUT);

		if (received == NULL || std::string(received).find("error") != std::string::npos)
			continue;

		parsedJson = nlohmann::json::parse(received);

		if (parsedJson.dump().find("@extra") == std::string::npos)
			continue;

		extraParameter = parsedJson.at("@extra").dump();

		if (extraParameter.empty() == false && extraParameter.find("checkLastChannelUpdates AuthorizationStateCheck") != std::string::npos) {
			if (parsedJson.at("@type").dump().find("authorizationStateReady") != std::string::npos)
				return true;
			return false;
		}
	}
}