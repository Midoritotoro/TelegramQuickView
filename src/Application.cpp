#include "Application.h"

#include "core/AutoRunUtils.h"

#include "ui/style/StyleCore.h"
#include "ui/style/StyleFont.h"


Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
{
    init();

    QString text = QString("https://www.google.com/\n"
        + QString("MSDN's primary web presence was at msdn.microsoft.com, and was a collection of sites for the developer community that provide information, documentation, and discussion that is authored both by Microsoft and by the community at large. Microsoft had placed emphasis on incorporation of forums, blogs, library annotations, and social bookmarking to make MSDN an open dialog with the developer community rather than a one-way service.[1] The main website, and most of its constituent applications below were available in 56[2] or more languages.\n")
        + "Library\n"
        + "The MSDN Library is the centralized repository of official developer - related documentation.Document sets are published by various user assistance organizations within Microsoft, and the community has the opportunity on many of the pages to add their own annotations.Community Content contributions can be edited by anyone.\n"
        + "MSDN Forums are the web - based forums used by the community to discuss a wide variety of software development topics.MSDN Forums were migrated to an all - new platform during 2008 that provided new features designed to improve efficiency such as inline preview of threads, AJAX filtering, and a slide - up post editor.\n"
        + "Blogs\n"
        + "MSDN blogs is a series of blogs that were hosted under Microsoft's domain blogs.msdn.com. Some blogs are dedicated to a product (e.g. Visual Studio, Internet Explorer, PowerShell) or a version of a product (e.g Windows 7, Windows 8), while others belong to a Microsoft employee (e.g. Michael Howard or Raymond Chen.\n"
        + "In May 2020, the MSDN and TechNet blogs were closed and the content was archived at Microsoft Docs.\n");

    qDebug() << text;
    QString videoPath = "C:\\Users\\danya\\Downloads\\2024-11-22_22-56-37.png";


   //_mediaPlayer = std::make_unique<MediaPlayer>();
    _messagesView = std::make_unique<MessagesView>();
    _messagesView->setMessageMediaDisplayMode(Message::MediaDisplayMode::Stack);

    _messagesView->show();

    auto list = QStringList({ videoPath });
    //QLabel* label = new QLabel;
    //label->setText(text);
    //label->show();
    //label->adjustSize();

   // qDebug() << fnt->ascent;

 //   _mediaPlayer->setMedia("C:/Users/danya/Downloads/videotestvertical.mp4");
  //  _mediaPlayer->show();
    _messagesView->makeMessage(text, list);
    
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

    if (!addToAutoRun())
        ExitProcess(1);

#endif // Q_OS_WIN

    setlocale(LC_ALL, "");

    const auto ratio = devicePixelRatio();
    const auto useRatio = std::clamp(qCeil(ratio), 1, 3);

    style::SetDevicePixelRatio(useRatio);

    style::SetCustomFont(u"OpenSans-Regular"_q);
    style::internal::StartFonts();

    style::RegisterStyles();

    style::Start();
    QPixmapCache::setCacheLimit(1024 * 1024); 
}