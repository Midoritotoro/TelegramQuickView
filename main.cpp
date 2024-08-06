﻿#ifdef _WIN32
    #ifndef UNICODE
        #define UNICODE
    #endif // UNICODE

    #ifndef _UNICODE
        #define _UNICODE
    #endif // _UNICODE
#endif // _WIN32

#ifdef __unix__
    #pragma message("Невозможно запустить на UNIX системах")
#endif // __unix__

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

    view.addMessage("Test very long message. Tet very long message. Test very long message.");
    view.addMessage("Test very long message. Tet very long message. Test very long message.", QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg") }); // , QUrlList{QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\top.png")}
    view.addMessage("Test very long message. Tet very long message. Test very long message.", QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg") });
    view.addMessage("Test very long message. Tet very long message. Test very long message.", QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg") });
    view.addMessage("Test very long message. Tet very long message. Test very long message.", QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });


    view.show();
    return app.exec();
}
