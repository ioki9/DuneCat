#include "essentialheaders.h"
#include "dcsettings.h"
#include "wmiclient.h"
#include "dcstunclient.h"
#include "dctrackermanager.h"
#include "dcprocessinfo.h"
#include "dcprocesstracker.h"
#include "dcprocesstablemodel.h"
#include "dcsortfilterprocessmodel.h"
#include "wmieventsink.h"
#include <QQuickStyle>
#include <iostream>

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

    qRegisterMetaType<DCProcessInfo>("DCProcessInfo");
    //quint16 port = static_cast<quint16>(QUrl("udp://tracker.openbittorrent.com:6969/announce").port());
    //QHostAddress host=QHostInfo::fromName(QUrl("udp://tracker.openbittorrent.com:6969/announce").host()).addresses()[0];
    //std::unique_ptr<DCTrackerManager> manager =
    //        std::make_unique<DCTrackerManager>(DCEndPoint{host,port});
    //manager->open_connection(DCEndPoint{QHostAddress("37.139.120.14"),3478});

    qputenv("QT_QUICK_CONTROLS_CONF",":/DuneCat/imports/qtquickcontrols2.conf");
    qmlRegisterType<DCProcessTableModel>("TableModels",1,0,"ProcessTableModel");
    qmlRegisterType<DCSortFilterProcessModel>("TableModels",1,0,"SortFilterProcessModel");
    qmlRegisterSingletonType(QUrl("qrc:/DuneCat/imports/qml/DCStyle.qml"), "DCStyle", 1, 0, "DCStyle");
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << QString(":/DuneCat/imports/icons"));
    QIcon::setThemeName("Default");
    QQmlApplicationEngine engine;
    engine.addImportPath(":/DuneCat/imports/qml");
    engine.addImportPath(":/DuneCat/imports/qml/controls");
    engine.addImportPath(":/DuneCat/imports/qml/pages");
    const QUrl url(u"qrc:/DuneCat/imports/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

