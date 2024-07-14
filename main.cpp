﻿#include "src/core/winapi/AutoRunRegistryUtils.h"

#include <QApplication>
#include "src/gui/settings/MainWindow.h"
#include "src/gui/panel/MessageWidget.h"


int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
    //if (!addParserToRegistryAutoRun())
     //  return -1;

    QApplication app(argc, argv);
    MessageWidget messageWidget;

    messageWidget.show();
    return app.exec();
}