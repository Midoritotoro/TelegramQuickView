﻿#ifdef _WIN32
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

    QString text = "Дверь, ведущая в недра планеты, открылась с легким скрипом. Пыль, застоявшаяся за миллионы лет, взметнулась, окутывая нас облаком времени. Профессор Лион, мой компаньон в этом безумном приключении, уже забирался внутрь, его глаза сияли детским восторгом. Я, Эмили, его ассистентка, исследовательница, и просто любопытная душа, следовала за ним, сдерживая дрожь, пробежавшую по спине.";

    TelegramPostQuickView* view = new TelegramPostQuickView();
    view->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::Stack);

    view->makeMessage("Username1", text);
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test8.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test8.jpg") });

    view->show();
    return app.exec();
}
