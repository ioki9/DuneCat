#include "essentialheaders.h"
#include "processtablemodel.h"
#include "sortfilterprocessmodel.h"
#include "sqlsortfiltermodel.h"
#include <QQuickStyle>
#include <QQmlContext>
#include "dbmanager.h"
#include "processinfo.h"
#include "sqltablemodel.h"
#include "sqlite3/sqlite3.h"
#include "globalsignalemitter.h"
#include "datevalidator.h"
#include "intvalidator.h"
using namespace DuneCat;

//Creates db connection with all the necessary db table stuff.
//Connects process creation/termination signals to store them in a database.
bool init_connect_db();
SqlSortFilterModel* create_proc_history_model();

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QTranslator translator;
    app.setApplicationName(QStringLiteral("DuneCat"));
    app.setOrganizationName(QStringLiteral("DuneCat"));
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "DuneCat_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    if(!init_connect_db())
        app.exit(-1);
    qputenv("QT_QUICK_CONTROLS_CONF",":/DuneCat/qtquickcontrols2.conf");

    qRegisterMetaType<ProcessInfo>("ProcessInfo");
    qmlRegisterType<DateValidator>("DCComponents", 1, 0, "DCDateValidator");
    qmlRegisterType<IntValidator>("DCComponents", 1, 0, "DCIntValidator");
    qmlRegisterType<ProcessTableModel>("DCTableModels",1,0,"ProcessTableModel");
    qmlRegisterType<SortFilterProcessModel>("DCTableModels",1,0,"SortFilterProcessModel");
    qmlRegisterSingletonType(QUrl(QStringLiteral("qrc:/DuneCat/qml/DCStyle.qml")), "DCStyle", 1, 0, "DCStyle");

    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << QStringLiteral(":/DuneCat/icons"));
    QIcon::setThemeName(QStringLiteral("Default"));
    QIcon icon = QIcon::fromTheme("desktop");
    qDebug()<<QIcon::themeName();
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("ProcessHistoryModel",create_proc_history_model());
    engine.addImportPath(QStringLiteral(":/DuneCat/qml"));
    engine.addImportPath(QStringLiteral(":/DuneCat/qml/controls"));
    engine.addImportPath(QStringLiteral(":/DuneCat/qml/pages"));
    engine.addImportPath(QStringLiteral(":/DuneCat/qml/components"));
    const QUrl url(u"qrc:/DuneCat/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

bool init_connect_db()
{
    ProcessTracker* tracker = ProcessTracker::get_instance();
    static DBManager db {"proc_history_write"};
    if(!db.open())
        return false;

    static QSqlQuery query_create{db.get_database()};
    static QSqlQuery query_delete{db.get_database()};
    bool res = db.set_journal_mode(JournalMode::WAL);
    if(!res)
    {
        qFatal()<<"Couldn't set database journal mode to WAL";
        return false;
    }
    res = query_create.exec(QStringLiteral("CREATE TABLE if NOT EXISTS processes_history "
                                                "(name TEXT NOT NULL,"
                                                " pid INTEGER NOT NULL,"
                                                " path TEXT,"
                                                " creation_time INTEGER NOT NULL,"
                                                " termination_time INTEGER,"
                                                " description TEXT,"
                                                " user TEXT,"
                                                " domain TEXT,"
                                                " cmd TEXT )"));
    if(!res)
    {
        qFatal()<<"Couldn't create the table. Error: "<<query_create.lastError().text();
        return false;
    }

    res = query_create.exec(QStringLiteral("CREATE UNIQUE INDEX if NOT EXISTS unique_pid_time ON processes_history (pid,creation_time)"));
    if(!res)
    {
        qFatal()<<"Couldn't create unique index. Error: "<<query_create.lastError().text();
        return false;
    }
    QSettings settings;
    QString last_alive = settings.value("LastAlive","").toString();
    settings.setValue("LastAlive",QDateTime::currentSecsSinceEpoch());
    if(!last_alive.isEmpty())
    {
        res = query_create.exec(QStringLiteral("UPDATE processes_history SET termination_time = '-" ) + last_alive
                                + QStringLiteral("' WHERE termination_time = 0;"));
        if(!res)
        {
            qFatal()<<"Couldn't update termination time from last active timestamp. Error: "<<query_create.lastError().text();
            return false;
        }
    }

    res = query_create.prepare(QStringLiteral("INSERT OR IGNORE INTO "
                                              "processes_history(name, pid, path, creation_time, termination_time, description, user, domain, cmd) "
                                              "VALUES(:name, :pid, :path, :creation_time, :termination_time, :description, :user, :domain, :cmd)"));
    if(!res)
    {
        qFatal()<<"Couldn't prepare the query for process creation. Error: "<<query_create.lastError().text();
        return false;
    }

    res = query_delete.prepare(QStringLiteral("UPDATE processes_history SET termination_time = :termination_time "
                                              "WHERE creation_time = :creation_time AND pid = :pid"));
    if(!res)
    {
        qFatal()<<"Couldn't prepare the query for process deletion. Error: "<<query_delete.lastError().text();
        return false;
    }


    auto start = std::chrono::steady_clock::now();
    tracker->apply_to_array([](const std::vector<ProcessInfo>& processes){
        if(db.transaction())
        {
            for(const auto& proc : processes)
            {
                qint64 creation_time = proc.creation_time.isValid() ? proc.creation_time.toSecsSinceEpoch() : 0;
                query_create.bindValue(0,proc.name);
                query_create.bindValue(1,proc.pid);
                query_create.bindValue(2,proc.file_path);
                query_create.bindValue(3,creation_time);
                query_create.bindValue(4,{0});
                query_create.bindValue(5,proc.description);
                query_create.bindValue(6,proc.owner_user);
                query_create.bindValue(7,proc.owner_domain);
                query_create.bindValue(8,proc.command_line);
                if(!query_create.exec())
                    qWarning()<<"Couldn't insert new process into history_processes table. Error: "
                               <<query_create.lastError().text();
            }
            if(!db.commit())
                db.print_last_db_error(QStringLiteral("Couldn't commit changes to history_processes table. Error: "));
        }
        else
            db.print_last_db_error(QStringLiteral("Couldn't start a transaction for initial query to process_history. Error:"));

    });
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    qDebug()<<elapsed_seconds.count();
    QObject::connect(tracker,&ProcessTracker::process_created,[](const ProcessInfo& proc){
      //  db.transaction();
        query_create.bindValue(0,proc.name);
        query_create.bindValue(1,proc.pid);
        query_create.bindValue(2,proc.file_path);
        query_create.bindValue(3,proc.creation_time.toSecsSinceEpoch());
        query_create.bindValue(4,{0});
        query_create.bindValue(5,proc.description);
        query_create.bindValue(6,proc.owner_user);
        query_create.bindValue(7,proc.owner_domain);
        query_create.bindValue(8,proc.command_line);
        if(!query_create.exec())
            qWarning()<<"Couldn't insert new process into history_processes table. Error: "<<query_create.lastError().text()
                       <<". Values are pid:"<<proc.pid<<" creation time:" << proc.creation_time.toSecsSinceEpoch();;
     //   else
     //       db.commit();
    });
    QObject::connect(tracker,&ProcessTracker::process_deleted,[](const ProcessInfo& proc){
        //db.transaction();
        query_delete.bindValue(QStringLiteral(":termination_time"),proc.termination_time.toSecsSinceEpoch());
        query_delete.bindValue(QStringLiteral(":creation_time"),proc.creation_time.toSecsSinceEpoch());
        query_delete.bindValue(QStringLiteral(":pid"),proc.pid);
        if(!query_delete.exec())
            qWarning()<<"Couldn't update termination date. Error: "<<query_delete.lastError().text()
                       <<". Values are pid:"<<proc.pid<<" creation time:" << proc.creation_time.toSecsSinceEpoch();
//        else
//            db.commit();
    });
    return true;
}

SqlSortFilterModel* create_proc_history_model()
{
    static DBManager db{"proc_history_read"};
    db.open();

    SqlTableModel* proc_history_model = new SqlTableModel(db,QApplication::instance());
    QString query;
    std::vector<QVariant> header;
    #ifndef Q_OS_LINUX
        query = QStringLiteral("SELECT name,pid,description,creation_time,termination_time FROM processes_history");
        header = {"Name", "PID", "Description" , "Creation time", "Termination time"};
    #else
        query = QStringLiteral("SELECT name,pid,creation_time,termination_time FROM processes_history");
        header = {"Name", "PID", "Creation time", "Termination time"};
    #endif
    proc_history_model->setQuery(query);
    proc_history_model->setHeaderData(header);

    QObject::connect(GlobalSignalEmitter::get_instance(),&GlobalSignalEmitter::db_wal_checkpoint,proc_history_model,
                     [proc_history_model](){proc_history_model->refresh();});

    SqlSortFilterModel* proc_history_sort_model = new SqlSortFilterModel(db.get_database(),QApplication::instance());
    proc_history_sort_model->setSourceModel(proc_history_model);
    return proc_history_sort_model;
}
