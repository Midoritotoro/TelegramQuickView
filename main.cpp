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
#include <iostream>

#include <QApplication>
#include "src/panel/TelegramPostQuickView.h"
#include "src/settings/MainWindow.h"
#include "src/media/player/MediaPlayer.h"
#include "src/media/player/MediaPlayerPanel.h"
#include "src/settings/TelegramAuthorizer.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow window;

    window.show();
    return app.exec();
    //TelegramAuthorizer auth;
    //auth.setTelegramCredentials(TelegramCredentials());
    //auth.loop();
    //while (true)
    //    std::cout << "test" << std::endl;
    //return 0;
}

//int main(int argc, char* argv[])
//{
//    AllocConsole();
//
//#ifdef _WIN32
//
//    if (!IS_MINIMUM_WINDOWS_VERSION) {
//        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
//        return -1;
//    }
//
//    if (!addParserToAutoRun()) {
//        MessageBox(NULL, L"Произошла ошибка при добавлении парсера Телеграм в автозагрузку", L"Ошибка", MB_OK);
//        return -1;
//    }
//
//#endif // _WIN32
//
//    QApplication app(argc, argv);
//    QString text = "🚩Загадка. Сколько здоровья у вороны?) \n Поломанный герой, который бустит свои статы не от базы, а от фактического здоровья во второй форме, в следствии чего выбивает огромные цифры урона и живет дольше некоторых боссов 🔫 ауф, дайте парочку таких\n И отсутствие перезарядки на воскрешение при убийстве - это отдельный вид искусства.Не надо сравнивать с Маричкой, она в соло команды не выносит и ее можно убить одним героем.";
//
//    TelegramPostQuickView* view = new TelegramPostQuickView();
//    view->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::PreviewWithCount);
//
//   // for (int index = 0; index < 1000; ++index)
//    view->makeMessage(text, QUrlList({ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\slipknot-the-blister-exists-official-music-video_(videomega.ru).mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\videotesthorizontal.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\raid.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\videotestvertical.mp4"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") }));
//
//    view->show();
//    return app.exec();
//}