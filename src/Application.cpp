#include "Application.h"

#include "core/AutoRunUtils.h"
#include "core/StyleCore.h"
#include "src/panel/TelegramPostQuickView.h"


Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
{
    init();

    QString text = "🚩Загадка. Сколько здоровья у вороны?)\n Поломанный герой, который бустит свои статы не от базы, а от фактического здоровья во второй форме, в следствии чего выбивает огромные цифры урона и живет дольше некоторых боссов 🔫 ауф, дайте парочку таких\n И отсутствие перезарядки на воскрешение при убийстве - это отдельный вид искусства.Не надо сравнивать с Маричкой, она в соло команды не выносит и ее можно убить одним героем.";
   
    QString path = "C:\\Users\\danya\\Downloads\\2024-11-22_22-56-37.png";
    QString path2 = "C:\\Users\\danya\\Downloads\\test1.jpg";
    QString path3 = "C:\\Users\\danya\\Downloads\\test2.jpg";

    QString path4 = "C:\\Users\\danya\\Downloads\\test3.jpg";
    QString path5 = "C:\\Users\\danya\\Downloads\\test4.jpg";
    QString path6 = "C:\\Users\\danya\\Downloads\\top.png";

    _postsView = std::make_unique<TelegramPostQuickView>();
    _postsView->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::PreviewWithCount);

    _postsView->show();

    auto list = QStringList({ path, path2, path3, path4, path5, path6 });
    auto list2 = QStringList({ path2, path, path3, path4, path5, path6 });
    auto list3 = QStringList({ path3, path2, path, path4, path5, path6 });
    auto list4 = QStringList({ path4, path2, path3, path, path5, path6 });
    auto list5 = QStringList({ path5, path2, path3, path4, path, path6 });
    auto list6 = QStringList({ path6, path2, path3, path4, path5, path });

    _postsView->makeMessage(text, list);
    _postsView->makeMessage(text, list2);
    _postsView->makeMessage(text, list3);
    _postsView->makeMessage(text, list4);
    _postsView->makeMessage(text, list5);
    _postsView->makeMessage(text, list6);
}

Application::~Application() {

}

std::unique_ptr<Application> Application::Create(int argc, char* argv[]) {
    return std::make_unique<Application>(argc, argv);
}

void Application::init() {
#ifdef Q_OS_WIN
    if (!IS_MINIMUM_WINDOWS_VERSION)
        ExitProcess(1);

    if (!addParserToAutoRun())
        ExitProcess(1);

#endif // Q_OS_WIN

    setlocale(LC_ALL, "");

    const auto ratio = devicePixelRatio();
    const auto useRatio = std::clamp(qCeil(ratio), 1, 3);

    style::SetDevicePixelRatio(useRatio);

    setFont(QFont("OpenSans-SemiBoldItalic", 10));
}