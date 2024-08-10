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
#include "src/settings/MainWindow.h"
#include "src/panel/TelegramPostQuickView.h"

int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
  /*  if (!addParserToRegistryAutoRun())
        return -1;*/

    QApplication app(argc, argv);
    TelegramPostQuickView view;

    QString text = "Test very long message. Tet very long message. Test very long message.";

    view.makeMessage("Username1", text);
    view.makeMessage("Username1", "", QUrlList{QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"),QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });
    view.makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg") });
    view.makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg") });
    view.makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });
    for (int index = 0; index < 100; index++) {
        view.makeMessage(text);
    }

    view.show();
    return app.exec();
}
