#include "EssentialHeaders.h"
#include <QLocale>
#include <QTranslator>
#include "DCSettings.h"
#include "DCStunClient.h"
#include "dctrackermanager.h"

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
    quint16 port = static_cast<quint16>(QUrl("udp://public.popcorn-tracker.org:6969").port());
    QHostAddress host=QHostInfo::fromName(QUrl("udp://opentracker.i2p.rocks:6969").host()).addresses()[0];
    std::unique_ptr<DCTrackerManager> manager =
            std::make_unique<DCTrackerManager>(DCEndPoint{host,port});
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
