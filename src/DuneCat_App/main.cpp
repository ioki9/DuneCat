#include "EssentialHeaders.pch"
#include <QGuiApplication>
#include <QLocale>
#include <QTranslator>
#include "DCSettings.h"
#include "../DuneCat_Network/DCStunClient.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "DuneCat_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    QHostAddress host("77.72.169.212");
    std::unique_ptr<DCStunClient> client =
            std::make_unique<DCStunClient>(DCEndPoint{host,3478});
    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:DuneCat/qml/DCBase/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
