#ifdef _WIN32
    #ifndef UNICODE
        #define UNICODE
    #endif // UNICODE

    #ifndef _UNICODE
        #define _UNICODE
    #endif // _UNICODE
#endif // _WIN32

#ifdef __linux__
    #pragma message("Невозможно запустить на Linux")
#endif // __linux__

#include "src/core/AutoRunUtils.h"

#include <QApplication>
#include "src/panel/TelegramPostQuickView.h"
#include "src/settings/MainWindow.h"
#include "src/media/player/MediaPlayer.h"

#include "src/core/StyleCore.h"

int main(int argc, char* argv[])
{
#ifdef _WIN32

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }

    /*if (!addParserToAutoRun()) {
        MessageBox(NULL, L"Произошла ошибка при добавлении парсера Телеграм в автозагрузку", L"Ошибка", MB_OK);
        return -1;
    }*/

#endif // _WIN32

    QApplication app(argc, argv);

    const auto ratio = app.devicePixelRatio();

    const auto useRatio = std::clamp(qCeil(ratio), 1, 3);
    style::setDevicePixelRatio(useRatio);
    
    QString text = "🚩Загадка. Сколько здоровья у вороны?)\n Поломанный герой, который бустит свои статы не от базы, а от фактического здоровья во второй форме, в следствии чего выбивает огромные цифры урона и живет дольше некоторых боссов 🔫 ауф, дайте парочку таких\n И отсутствие перезарядки на воскрешение при убийстве - это отдельный вид искусства.Не надо сравнивать с Маричкой, она в соло команды не выносит и ее можно убить одним героем.";

    TelegramPostQuickView* view = new TelegramPostQuickView();
    view->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::Stack);

    QUrlList list({ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg")});
    
    for (int i = 0; i < 100; ++i)
        view->makeMessage(text, list);


    view->show();
    return app.exec();
}
