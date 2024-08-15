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
#include "src/settings/MainWindow.h"
#include "src/panel/TelegramPostQuickView.h"
#include "src/panel/MessageTextView.h"
#include <QElapsedTimer>


int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
    /*  if (!addParserToRegistryAutoRun())
          return -1;*/

    QElapsedTimer timer;
    timer.start();

    QApplication app(argc, argv);
    QString text = "Дверь, ведущая в недра планеты, открылась с легким скрипом. Пыль, застоявшаяся за миллионы лет, взметнулась, окутывая нас облаком времени. Профессор Лион, мой компаньон в этом безумном приключении, уже забирался внутрь, его глаза сияли детским восторгом. Я, Эмили, его ассистентка, исследовательница, и просто любопытная душа, следовала за ним, сдерживая дрожь, пробежавшую по спине.";

    TelegramPostQuickView* view = new TelegramPostQuickView();
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4") });
    text = "Test message 3. Test message 3. Test message 3. Test message 3. Test message 3.";
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"),QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"),  QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\videoRes.mp4") });
    /*text = "Test message 2";
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg") });
    text = "Test message 4";
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });

    for (int index = 0; index < 100; ++index) {
        view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"),QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"),  QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\videoRes.mp4") });
    }*/

    view->show();
    qint64 elapsed = timer.elapsed();
    qDebug() << "Time taken: " << static_cast<double>(elapsed) / 1000 << " s";

    return app.exec();
}
