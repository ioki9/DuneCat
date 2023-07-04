#include "essentialheaders.h"
#include "settings.h"
#include "processtablemodel.h"
#include "sortfilterprocessmodel.h"
#include <QQuickStyle>
#include "dbmanager.h"
#include "processinfo.h"
#include <chrono>

using namespace DuneCat;

//connects db to ProcessTracker signals
bool init_connect_db()
{
    ProcessTracker* tracker = ProcessTracker::get_instance();
    static DBManager db{"processes_conn","data.db"};
    db.open();

    static QSqlQuery query_create{db.get_database()};
    static QSqlQuery query_delete{db.get_database()};
    bool res = query_create.exec(QStringLiteral("CREATE TABLE if NOT EXISTS processes_history "
                                                 "(name TEXT NOT NULL,"
                                                 " pid INTEGER NOT NULL,"
                                                 " path TEXT,"
                                                 " creation_date INTEGER NOT NULL,"
                                                 " termination_date INTEGER,"
                                                 " description TEXT,"
                                                 " user TEXT,"
                                                 " domain TEXT,"
                                                 " cmd TEXT )"));
    if(!res)
    {
        qFatal()<<"Couldn't create the table. Error: "<<query_create.lastError().text();
        return false;
    }
    res = query_create.prepare(QStringLiteral("INSERT INTO "
        "processes_history(name, pid, path, creation_date, termination_date, description, user, domain, cmd) "
        "VALUES(:name, :pid, :path, :creation_date, :termination_date, :description, :user, :domain, :cmd)"));
    if(!res)
    {
        qFatal()<<"Couldn't prepare the query for process creation. Error: "<<query_create.lastError().text();
        return false;
    }

    res = query_delete.prepare(QStringLiteral("UPDATE processes_history SET termination_date = :termination_date "
                                        "WHERE creation_date = :creation_date AND pid = :pid"));
    if(!res)
    {
        qFatal()<<"Couldn't prepare the query for process deletion. Error: "<<query_delete.lastError().text();
        return false;
    }
    auto start = std::chrono::steady_clock::now();
    tracker->apply_to_every_elem([](const ProcessInfo& proc){
        query_create.bindValue(0,proc.name);
        query_create.bindValue(1,proc.pid);
        query_create.bindValue(2,proc.file_path);
        query_create.bindValue(3,proc.creation_date.toSecsSinceEpoch());
        query_create.bindValue(4,QStringLiteral("NULL"));
        query_create.bindValue(5,proc.description);
        query_create.bindValue(6,proc.owner_user);
        query_create.bindValue(7,proc.owner_domain);
        query_create.bindValue(8,proc.command_line);
        if(!query_create.exec())
            qWarning()<<"couldn't insert new process. Error: "<<query_create.lastError().text();
    });
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    qDebug()<<elapsed_seconds.count();
    QObject::connect(tracker,&ProcessTracker::process_created,[](const ProcessInfo& proc){
        query_create.bindValue(0,proc.name);
        query_create.bindValue(1,proc.pid);
        query_create.bindValue(2,proc.file_path);
        query_create.bindValue(3,proc.creation_date.toSecsSinceEpoch());
        query_create.bindValue(4,QStringLiteral("NULL"));
        query_create.bindValue(5,proc.description);
        query_create.bindValue(6,proc.owner_user);
        query_create.bindValue(7,proc.owner_domain);
        query_create.bindValue(8,proc.command_line);
        if(!query_create.exec())
            qWarning()<<"couldn't insert new process. Error: "<<query_create.lastError().text();
    });
    QObject::connect(tracker,&ProcessTracker::process_deleted,[](const ProcessInfo& proc){
        query_delete.bindValue(QStringLiteral(":termination_date"),proc.termination_date.toSecsSinceEpoch());
        query_delete.bindValue(QStringLiteral(":creation_date"),proc.creation_date.toSecsSinceEpoch());
        query_delete.bindValue(QStringLiteral(":pid"),proc.pid);
        if(!query_delete.exec())
            qWarning()<<"couldn't update termination date. Error: "<<query_delete.lastError().text();
    });
    return true;
}

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
    qputenv("QT_QUICK_CONTROLS_CONF",":/DuneCat/imports/qtquickcontrols2.conf");
    qRegisterMetaType<ProcessInfo>("ProcessInfo");
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

