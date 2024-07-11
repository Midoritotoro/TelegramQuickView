#include "PythonCaller.h"
#include <exception>
#include <iostream>


int main() {
	const char* PyModuleName = "TelegramRobber";
	try {
		PythonCaller* pythonCaller = new PythonCaller("D:\\TelegramQuickView\\src\\core\\parser\\");
		pythonCaller->CallTelegramParseFunction(PyModuleName, "C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media");
	}
	catch (const PyBaseExceptionObject& object) {
		std::cout << object.traceback << object.context << object.notes;
	}
	return 0;
}