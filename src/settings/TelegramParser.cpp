#include "TelegramParser.h"

#define TELEGRAM_CLIENT_WAIT_TIMEOUT 10


TelegramParser::TelegramParser(std::string apiHash, std::string apiId, std::string phoneNumber) {
	td_execute("{\"@type\":\"setLogVerbosityLevel\", \"new_verbosity_level\":3}");


	_tdClientId = td_create_client_id();
	_tdJsonClient = td_json_client_create();

	authorizeToAccount(apiHash, apiId, phoneNumber);
}

TelegramParser::~TelegramParser() {
	td_json_client_destroy(_tdJsonClient);
}

void TelegramParser::authorizeToAccount(std::string apiHash, std::string apiId, std::string phoneNumber) {
	nlohmann::json jsonParsed;
	std::string authorizationStatus;
	std::string code;

	std::string payloadSetTdlibParameters = "{ \"@type\": \"setTdlibParameters\","
		" \"api_id\": " + apiId + ", \"api_hash\": \"" + apiHash + "\"," +
		"\"use_test_dc\": false, \"device_model\": \"Desktop\", \"system_version\": \"Unknown\", \"application_version\": \"0.0\", \"system_language_code\": \"en\","
		"\"database_directory\": \"D:/TelegramCppParser/\", \"files_directory\": \"D:/TelegramCppParser/\", \"use_file_database\": true, \"use_chat_info_database\": true,"
		"\"use_message_database\": true, \"use_secret_chats\": false, \"enable_storage_optimizer\": true,\"ignore_file_names\": false}";

	std::string payloadSetAuthenticationPhoneNumber = "{ \"@type\": \"setAuthenticationPhoneNumber\", \"phone_number\": \"" + phoneNumber + "\" }";
	td_json_client_send(_tdJsonClient, "{ \"@type\": \"getAuthorizationState\", \"@extra\": \"1.01234\" }");

	while (true) {
		const char* received = td_json_client_receive(_tdJsonClient, TELEGRAM_CLIENT_WAIT_TIMEOUT);

		if (received == NULL
			|| std::string(received).find("error") != std::string::npos)
			continue;

		jsonParsed = nlohmann::json::parse(received);
		authorizationStatus = jsonParsed.at("@type").dump();
		std::cout << authorizationStatus << std::endl;

		if (authorizationStatus.find("authorizationStateClosed") != std::string::npos)
			// Клиент закрыт
				return;

		else if (authorizationStatus.find("authorizationStateWaitTdlibParameters") != std::string::npos) {
			// Необходимо ввести параметры TdLib
			td_json_client_send(_tdJsonClient, payloadSetTdlibParameters.c_str());
		}
		else if (authorizationStatus.find("authorizationStateWaitPhoneNumber") != std::string::npos) {
			// Необходимо ввести телефонный номер пользователя
			std::cout << payloadSetAuthenticationPhoneNumber << std::endl;
			td_json_client_send(_tdJsonClient, payloadSetAuthenticationPhoneNumber.c_str());
		}
		else if (authorizationStatus.find("authorizationStateWaitCode") != std::string::npos) {
			// Пользователю необходимо ввести код для входа в Телеграм
			std::cout << "Введите код для входа в Телеграм: " << std::endl;
			std::cin >> code;

			std::string payloadCheckAuthenticationCode = "{ \"@type\": \"checkAuthenticationCode\", \"code_\": \"" + code + "\" }";

			td_json_client_send(_tdJsonClient, payloadCheckAuthenticationCode.c_str());
		}

		if (authorizationStatus.find("authorizationStateReady") != std::string::npos) {
			// Вход выполнен успешно
			std::cout << "success" << std::endl;
			return;
		}
	}
}
