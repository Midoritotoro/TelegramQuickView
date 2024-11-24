#include "Application.h"

#include "core/AutoRunUtils.h"
#include "core/StyleCore.h"
#include "src/panel/TelegramPostQuickView.h"


Application::Application(int& argc, char** argv):
	QApplication(argc, argv)
{
	init();

    QString text = "🚩Загадка. Сколько здоровья у вороны?)\n Поломанный герой, который бустит свои статы не от базы, а от фактического здоровья во второй форме, в следствии чего выбивает огромные цифры урона и живет дольше некоторых боссов 🔫 ауф, дайте парочку таких\n И отсутствие перезарядки на воскрешение при убийстве - это отдельный вид искусства.Не надо сравнивать с Маричкой, она в соло команды не выносит и ее можно убить одним героем.";

    _postsView = std::make_unique<TelegramPostQuickView>();
    _postsView->setMessageMediaDisplayMode(MessageWidget::MessageMediaDisplayMode::PreviewWithCount);

    _postsView->show();
}

Application::~Application() {
    _postsView = nullptr;
}

std::unique_ptr<Application> Application::Create(int argc, char* argv[]) {
    return std::make_unique<Application>(argc, argv);
}

void Application::init() {
    const auto ratio = devicePixelRatio();
    const auto useRatio = std::clamp(qCeil(ratio), 1, 3);

    style::SetDevicePixelRatio(useRatio);

#ifdef Q_OS_WIN
    if (!IS_MINIMUM_WINDOWS_VERSION)
        ExitProcess(1);

    if (!addParserToAutoRun())
        ExitProcess(1);

#endif // Q_OS_WIN
}