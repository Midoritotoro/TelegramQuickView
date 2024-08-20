#ifdef _WIN32
    #ifndef UNICODE
        #define UNICODE
    #endif // UNICODE

    #ifndef _UNICODE
        #define _UNICODE
    #endif // _UNICODE
#endif // _WIN32

#ifdef __linux__
    #pragma message("Невозможно запустить на Linux системах")
#endif // __linux__

#include "src/core/AutoRunUtils.h"

#include <QApplication>
#include "src/core/MouseDetector.h"


int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }

    if (!addParserToRegistryAutoRun()) {
        MessageBox(NULL, L"Произошла ошибка при добавлении парсера Телеграм в автозагрузку", L"Ошибка", MB_OK);
        return -1;
    }

    QApplication app(argc, argv);

    MouseDetector* mouseDetector = new MouseDetector();
    mouseDetector->trackMouse();

    return app.exec();
}
