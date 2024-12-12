#include "Application.h"

#include "core/AutoRunUtils.h"
#include "view/MessagesView.h"
#include "core/StyleCore.h"


Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
{
    init();

    QString text = "Test text Click <a href=\"https://google.com\" style=\"color: blue\">here</a> for something special!</a>\n ВС РФ ударили по объектам энергетики в Тернополе и Ровенской области.\n. В Тернополе «Герань» атаковала критическую инфраструктуру(видео), сообщил мэр.Часть города осталась без света.Местные власти призывают население запастись водой и зарядить гаджеты \nТакже есть попадание по объекту энергетической инфраструктуры в Ровенской области, отчитались в ОВА(фото).\n 🇺🇦 Взрывы прогремели в Староконстантинове, Винницкой и Киевской областях.";
    QString videoPath = "C:\\Users\\danya\\Downloads\\2024-11-22_22-56-37.png";

    _messagesView = std::make_unique<MessagesView>();
    _messagesView->setMessageMediaDisplayMode(Message::MediaDisplayMode::PreviewWithCount);

    _messagesView->show();
    auto list = QStringList({ videoPath });

    _messagesView->makeMessage(text, list);
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

    QPixmapCache::setCacheLimit(1024 * 1024); 
    setFont(QFont("OpenSans-SemiBoldItalic", 10));
}