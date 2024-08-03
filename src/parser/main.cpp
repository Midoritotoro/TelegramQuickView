#include "TelegramRobber.h"
#include <Windows.h>

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
int main() {
	const char* PyModuleName = "TelegramRobber";
	TelegramRobber* telegramRobber = new TelegramRobber("D:\\TelegramQuickView\\src\\parser\\");
	telegramRobber->CallTelegramParseFunction(PyModuleName, "C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media");
	return 0;
}