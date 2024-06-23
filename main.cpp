#include <QApplication>

#pragma push_macro("slots")
#undef slots
#include "TelegramQuickView/src/core/parser/python/call/PythonCaller.h"
#pragma pop_macro("slots")

#include "TelegramQuickView/src/core/winapi/AutoRunRegistryUtils.h"
#include "TelegramQuickView/src/gui/settings/MainWindow.h"

int __cdecl main(int argc, char* argv[]) 
{
	if (!IS_MINIMUM_WINDOWS_VERSION)
		MessageBox(NULL, _T("Приложение работает на версиях Windows выше 10"), _T("Ошибка"), MB_OK);
	if (!addParserToRegistryAutoRun())
		return -1;

	const char* PyModuleName = "TelegramRobber";
	const char* TelegramUsername = "antifishechki";
	const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
	const char* phoneNumber = "+375292384917";
	const char* downloadPath = "D:\\Media";
	const char* pythonFilePath = "./TelegramQuickView/src/core/parser/python/code/";
	long long apiId = 13711370;
	PythonCaller pythonCaller(apiHash, phoneNumber, downloadPath, apiId, pythonFilePath);
	pythonCaller.CallTelegramParseFunction(PyModuleName, TelegramUsername, "2024-06-14", "2024-06-24");

	QApplication app(argc, argv);
	MainWindow window;
	window.show();
	return app.exec();

	return 0;
}
