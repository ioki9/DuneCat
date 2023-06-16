#include "essentialheaders.h"
#include "settings.h"
#include "processtablemodel.h"
#include "sortfilterprocessmodel.h"
#include <QQuickStyle>

using namespace DuneCat;

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

    qRegisterMetaType<ProcessInfo>("ProcessInfo");
    //quint16 port = static_cast<quint16>(QUrl("udp://tracker.openbittorrent.com:6969/announce").port());
    //QHostAddress host=QHostInfo::fromName(QUrl("udp://tracker.openbittorrent.com:6969/announce").host()).addresses()[0];
    //std::unique_ptr<TrackerManager> manager =
    //        std::make_unique<TrackerManager>(EndPoint{host,port});
    //manager->open_connection(EndPoint{QHostAddress("37.139.120.14"),3478});

    qputenv("QT_QUICK_CONTROLS_CONF",":/DuneCat/imports/qtquickcontrols2.conf");
    qmlRegisterType<ProcessTableModel>("TableModels",1,0,"ProcessTableModel");
    qmlRegisterType<SortFilterProcessModel>("TableModels",1,0,"SortFilterProcessModel");
    qmlRegisterSingletonType(QUrl(QStringLiteral("qrc:/DuneCat/imports/qml/DCStyle.qml")), "DCStyle", 1, 0, "DCStyle");
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << QStringLiteral(":/DuneCat/imports/icons"));
    QIcon::setThemeName(QStringLiteral("Default"));
    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral(":/DuneCat/imports/qml"));
    engine.addImportPath(QStringLiteral(":/DuneCat/imports/qml/controls"));
    engine.addImportPath(QStringLiteral(":/DuneCat/imports/qml/pages"));
    engine.addImportPath(QStringLiteral(":/DuneCat/imports/qml/components"));
    const QUrl url(u"qrc:/DuneCat/imports/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

