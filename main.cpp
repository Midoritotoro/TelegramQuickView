﻿#ifdef _WIN32
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

#include <QApplication>
#include "src/settings/MainWindow.h"
#include "src/panel/TelegramPostQuickView.h"
#include "src/panel/MessageTextView.h"
#include <QFont>
#include <QStringLiteral>

int main(int argc, char* argv[])
{
    AllocConsole();

    if (!IS_MINIMUM_WINDOWS_VERSION) {
        MessageBox(NULL, L"Приложение работает на версиях Windows от 10 и выше", L"Ошибка", MB_OK);
        return -1;
    }
  /*  if (!addParserToRegistryAutoRun())
        return -1;*/

    QApplication app(argc, argv);
    QString text = QStringLiteral(u"Дверь, ведущая в недра планеты, открылась с легким скрипом. Пыль, застоявшаяся за миллионы лет, взметнулась, окутывая нас облаком времени. Профессор Лион, мой компаньон в этом безумном приключении, уже забирался внутрь, его глаза сияли детским восторгом. Я, Эмили, его ассистентка, исследовательница, и просто любопытная душа, следовала за ним, сдерживая дрожь, пробежавшую по спине."
        u"Температура резко понизилась, воздух стал влажным и тяжелым. Повсюду царила тишина, нарушаемая только нашим дыханием и стуком наших ботинок по твердому грунту. Факелы, которые мы держали в руках, едва пробивали мрак, оставляя за собой зыбкие тени, танцующие на стенах пещеры."
        u"Мы шли по узкому туннелю, спускающемуся все глубже и глубже. Стены пещеры были покрыты странными узорами, напоминающими окаменелые растения, и отблески факелов создавали причудливые картины. Профессор Лион, захваченный увиденным, бормотал себе под нос какие-то непонятные слова, погруженный в свои мысли."
        u"Эмили, смотрите! - он указал на стену, где в тусклом свете факела мерцало что-то похожее на кристалл. Это, должно быть, остатки древней цивилизации!"
        u"Мы подошли ближе, и, действительно, на кристалле были выгравированы странные символы, напоминающие иероглифы. Я не понимала, что они означают, но чувствовала, что они хранят в себе тайну, которая откроет нам неизвестные грани истории.nn"
        u"Мы продолжали свой путь, продвигаясь все глубже в недра Земли. Вокруг нас царила необыкновенная тишина, нарушаемая лишь тихим шумом капающей воды, словно плач Земли по своим потерянным секретам."
        u"Вдруг, путь преградил огромный камень, полностью перекрывший туннель.nn"
        u"Неужели это конец ? - я задала вопрос, который, кажется, витал в воздухе.nn"
        u"Профессор Лион, как всегда, оптимистично улыбнулся. Нет, моя дорогая Эмили, мы не сдаемся так легко.Нужно найти другой путь!"
        u"Он присмотрелся к стене, ищуя какие-нибудь следы, которые могли бы указать нам направление.nn"
        u"Смотрите, вот здесь, небольшая щель! - он указал на едва заметный проход, скрытый за каменным выступом.nn"
        u"Мы пролезли в щель, и она вела нас в новую, более просторную пещеру.  Она была освещена мягким светом, исходящим от кристаллических образований на стенах. В центре пещеры находился огромный, изящный купол, состоящий из переливающихся кристаллов.nn"
        u"Это невероятно! - воскликнула я, завороженная красотой этого места."
        u"Да, Эмили, мы, кажется, нашли то, что искали! - профессор Лион, взволнованный, уже направлялся к куполу."
        u"Мы приблизились к куполу, и, чем ближе мы были к нему, тем ярче и насыщеннее становился свет, исходящий от кристаллов. Я чувствовала, что мы приближаемся к чему-то особенному, к тайне, которая может изменить мир.nn"
        u"Профессор Лион прикоснулся к одному из кристаллов, и он засветился ярким светом, проецируя на стену странные изображения, которые, казалось, оживали у нас на глазах.  nn"
        u"Это карта! - воскликнул профессор Лион, узнав знакомые символы, выгравированные на кристалле в начале нашего пути. Она ведет к центру Земли!"
        u"Мы, затаив дыхание, наблюдали, как изображения на стене меняются, показывая нам путь, ведущий к самой сердцевине планеты.nn"
        u"Мы должны идти, Эмили! - сказал профессор Лион, его глаза горели азартом. Мы должны увидеть, что скрывает центр Земли!"
        u"Я, вдохновленная его энтузиазмом, кивнула.  Мы  взяли  факелы и отправились в путь,  уверенные, что  наше  путешествие  только  начинается.nn"
        u"Впереди  нас  ждал  неизвестный  мир,  полный  тайны  и  опасности,  но  мы  были  готовы  открыть  его  секреты."
    );


    TelegramPostQuickView* view = new TelegramPostQuickView();
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\gift.mp4") });
    text = "Test message 3";
    view->makeMessage("Username1", text, QUrlList{QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test1.jpg"),QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test2.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg"), QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg"),  QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\videoRes.mp4") });
    text = "Test message 2";
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test3.jpg") });
    text = "Test message 4";
    view->makeMessage("Username1", text, QUrlList{ QUrl::fromLocalFile("C:\\Users\\danya\\Downloads\\test4.jpg") });

    view->show();

    return app.exec();
}
