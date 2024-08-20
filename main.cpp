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
    /*  if (!addParserToRegistryAutoRun())
          return -1;*/

    QApplication app(argc, argv);

    //MouseDetector* mouseDetector = new MouseDetector();
    //mouseDetector->trackMouse();

    QString text = "Дверь, ведущая в недра планеты, открылась с легким скрипом. Дверь, ведущая в недра планеты, открылась с легким скрипом. 1 Дверь, ведущая в недра планеты, открылась с легким скрипом. 2 Дверь, ведущая в недра планеты, открылась с легким скрипом. 3 Дверь, ведущая в недра планеты, открылась с легким скрипом. 4 Дверь, ведущая в недра планеты, открылась с легким скрипом. 5 Дверь, ведущая в недра планеты, открылась с легким скрипом. 6 Дверь, ведущая в недра планеты, открылась с легким скрипом. 7 Дверь, ведущая в недра планеты, открылась с легким скрипом. 8 Дверь, ведущая в недра планеты, открылась с легким скрипом. 9Дверь, ведущая в недра планеты, открылась с легким скрипом. 10 Дверь, ведущая в недра планеты, открылась с легким скрипом. 11 Дверь, ведущая в недра планеты, открылась с легким скрипом. 12 Дверь, ведущая в недра планеты, открылась с легким скрипом. 13 Дверь, ведущая в недра планеты, открылась с легким скрипом. 14 Дверь, ведущая в недра планеты, открылась с легким скрипом. 15 Дверь, ведущая в недра планеты, открылась с легким скрипом. 16 Дверь, ведущая в недра планеты, открылась с легким скрипом. 17 Пыль, застоявшаяся за миллионы лет, взметнулась, окутывая нас облаком времени. Профессор Лион, мой компаньон в этом безумном приключении, уже забирался внутрь, его глаза сияли детским восторгом. Я, Эмили, его ассистентка, исследовательница, и просто любопытная душа, следовала за ним, сдерживая дрожь, пробежавшую по спине.";
   // text = "Дверь, ведущая в недра планеты, открылась с легким скрипом.Дверь, ведущая в недра планеты, открылась с легким скрипом.";

    TelegramPostQuickView* view = new TelegramPostQuickView();
    view->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::Stack);

    view->makeMessage("Username1", text);
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test8.jpg") });

    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test8.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test7.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg") });

    view->show();

    return app.exec();
}
