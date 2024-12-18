#include "Application.h"

#include "core/AutoRunUtils.h"

#include "ui/style/StyleCore.h"
#include "ui/style/StyleFont.h"


Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
{
    init();

    QString text = "Test text Click https://google.com ВС РФ ударили по объектам энергетики в Тернополе и Ровенской области.\n. В Тернополе «Герань» атаковала критическую инфраструктуру(видео), сообщил мэр.Часть города осталась без света.Местные власти призывают население запастись водой и зарядить гаджеты \nТакже есть попадание по объекту энергетической инфраструктуры в Ровенской области, отчитались в ОВА(фото).\n 🇺🇦 Взрывы прогремели в Староконстантинове, Винницкой и Киевской областях.";
    QString videoPath = "C:\\Users\\danya\\Downloads\\2024-11-22_22-56-37.png";


   //_mediaPlayer = std::make_unique<MediaPlayer>();
    _messagesView = std::make_unique<MessagesView>();
    _messagesView->setMessageMediaDisplayMode(Message::MediaDisplayMode::Stack);

    _messagesView->show();
    auto list = QStringList({ videoPath });
    //QLabel* label = new QLabel;
    //label->setFont(style::font(13, style::FontFlag::StrikeOut, 0));
    //label->setText(text);
    //label->show();
    //label->adjustSize();

    //_mediaPlayer->setMedia("C:/Users/danya/Downloads/videotestvertical.mp4");
    //_mediaPlayer->show();
     _messagesView->makeMessage(text);
    
}

Application::~Application() {
    style::Stop();
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

    style::SetCustomFont(u"OpenSans-Regular"_q);
    style::internal::StartFonts();

    style::Start();

    QPixmapCache::setCacheLimit(1024 * 1024); 
}