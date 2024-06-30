#include "call/PythonCaller.h"

int main() {
	const char* PyModuleName = "TelegramRobber";
	const char* TelegramUsername = "antifishechki";
	const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
	const char* phoneNumber = "+375292384917";
	const char* downloadPath = "D:\\Media";
	const char* pythonFilePath = "code/TelegramRobber.py";
	long long apiId = 13711370;
	PythonCaller pythonCaller(apiHash, phoneNumber, downloadPath, apiId, pythonFilePath);
	pythonCaller.CallTelegramParseFunction(PyModuleName, TelegramUsername, "2024-05-04", "2024-05-05");
	return 0;
}