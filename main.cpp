#include "src/core/winapi/AutoRunRegistryUtils.h"

#include <QtQml>
#include <QGuiApplication>
#include <QApplication>
#include <Windows.h>


#include "src/gui/settings/MainWindow.h"
#include "src/core/sql/SqlConversationModel.h"
#include "src/core/sql/SqlConnect.h"
#include "src/gui/settings/TelegramAuthorizationChecker.h"

int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
    if (!addParserToRegistryAutoRun())
        return -1;

   // QApplication app(argc, argv);
   // MainWindow window;

   // window.show();
    //return app.exec();
    TelegramAuthorizationChecker* checker = new TelegramAuthorizationChecker();
    bool isTelegramCodeValid = checker->sendTelegramCode("019edf3f20c8460b741fb94114e6fec0", "+375292384917", 13711370);
    return 0;
}

// apiHash = "019edf3f20c8460b741fb94114e6fec0";
// phoneNumber = "+375292384917";
// apiId = 13711370;