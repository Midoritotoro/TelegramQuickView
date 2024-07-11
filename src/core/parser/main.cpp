#include "PythonCaller.h"


int main() {
	const char* PyModuleName = "TelegramRobber";
	PythonCaller* pythonCaller = new PythonCaller("D:\\TelegramQuickView\\src\\core\\parser\\");
	pythonCaller->CallTelegramParseFunction(PyModuleName, "C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media");
	return 0;
}