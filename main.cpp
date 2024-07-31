#ifdef _WIN32
    #ifndef UNICODE
        #define UNICODE
    #endif // UNICODE

    #ifndef _UNICODE
        #define _UNICODE
    #endif // _UNICODE
#endif // _WIN32

#ifdef __unix__
    #pragma message("Невозможно запустить на UNIX системах")
#endif // __unix__

#include "src/core/winapi/AutoRunRegistryUtils.h"

#include <QApplication>
#include "src/gui/settings/MainWindow.h"
#include "src/gui/panel/MessageWidget.h"

int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
    //if (!addParserToRegistryAutoRun())
     //  return -1;

    QApplication app(argc, argv);

    const QString text = "Очень длинный текст сообщения. Очень длинный текст сообщения. Очень длинный текст сообщения.Очень длинный текст сообщения.Очень длинный текст сообщения.Очень длинный текст сообщения.";
    const QString path = "C:\\Users\\danya\\Downloads\\top.png";
    const QString path2 = "C:\\Users\\danya\\Downloads\\videoRes.mp4";
    QUrlList list = {  };
    QUrlList list2 = {  };
    
    MessageWidget messageWidget(text, list);
    list.append(QUrl::fromLocalFile(path));
    messageWidget.setSource(text, list);
 
    list2.append(QUrl::fromLocalFile(path2));
    messageWidget.setSource(text, list2);

    messageWidget.show();
    return app.exec();
}