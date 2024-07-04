#include "src/core/winapi/AutoRunRegistryUtils.h"

#include <QtQml>
#include <QGuiApplication>
#include <QApplication>
#include <Windows.h>


#include "src/gui/settings/MainWindow.h"
#include "src/core/sql/SqlConversationModel.h"
#include "src/core/sql/SqlConnect.h"
#include "src/core/parser/PythonCaller.h"

int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
    if (!addParserToRegistryAutoRun())
        return -1;

    QApplication app(argc, argv);
    MainWindow window;

    PythonCaller* p = new PythonCaller("", "", 1, "");
    bool r = p->callTelegramAuthorizeCheck();

    window.show();
    return app.exec();
}
