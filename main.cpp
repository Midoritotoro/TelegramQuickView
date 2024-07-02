#include "src/core/sql/SqlConversationModel.h"
#include "src/core/sql/SqlConnect.h"
#include "src/core/winapi/AutoRunRegistryUtils.h"
#include "src/gui/settings/MainWindow.h"


#include <QtQml>
#include <QGuiApplication>
#include <QApplication>
#include <Windows.h>


int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
    if (!addParserToRegistryAutoRun())
        return -1;

    //QGuiApplication app(argc, argv);

    //qmlRegisterType<SqlConversationModel>("sql.SqlConversationModel", 1, 0, "SqlConversationModel");
    //connectToDatabase();

    //QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("../../src/gui/qml/main.qml")));
    //if (engine.rootObjects().isEmpty())
    //    return -1;

    QApplication app(argc, argv);
    MainWindow window;

    window.show();
    return app.exec();
}
