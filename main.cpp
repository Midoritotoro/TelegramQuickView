#include <QApplication>

#pragma push_macro("slots")
#undef slots
#include "TelegramQuickView/src/core/parser/python/call/PythonCaller.h"
#pragma pop_macro("slots")

#include "TelegramQuickView/src/core/winapi/AutoRunRegistryUtils.h"
#include "TelegramQuickView/src/gui/core/ScrollBar.h"

int __cdecl main(int argc, char* argv[]) 
{
	AllocConsole();
	if (!IS_MINIMUM_WINDOWS_VERSION) {
		MessageBox(NULL, L"Приложение работает на версиях Windows выше 10", L"Ошибка", MB_OK);
		return -1;
	}
	if (!addParserToRegistryAutoRun())
		return -1;


	QApplication app(argc, argv);
	ScrollBar window;
	window.show();
	return app.exec();
}
