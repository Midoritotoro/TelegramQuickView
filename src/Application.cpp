#include "Application.h"

#include "core/AutoRunUtils.h"
#include "core/StyleCore.h"

#include "src/panel/TelegramPostQuickView.h"
#include "src/media/player/MediaPlayer.h"


Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
{
    init();

    QString text = "Test text";
    QString videoPath = "C:\\Users\\danya\\Downloads\\raidvid.mp4";

    _postsView = std::make_unique<TelegramPostQuickView>();
    _postsView->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::Stack);

    _postsView->show();

    auto list = QStringList({ videoPath });

    _postsView->makeMessage(text, list);
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