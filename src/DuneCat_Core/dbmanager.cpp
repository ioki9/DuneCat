#include "dbmanager.h"
#include <mutex>
#include "sqlite3/sqlite3.h"
#include "globalsignalemitter.h"

namespace DuneCat
{
std::map<QString,std::atomic_int8_t> DBManager::m_open_connections_count{};
const QString DBManager::m_driver_name {QStringLiteral("QSQLITE")};

DBManager::DBManager(const QString& connection_name)
{
    m_db = create_connection(connection_name);
    if(m_db.isValid())
        m_db.setDatabaseName(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/data.db"));
    constexpr int msec = 3 * 60 * 60 * 1000;
    m_timer.start(msec);
    QObject::connect(&m_timer,&QTimer::timeout,QCoreApplication::instance(),[this](){
        QSqlQuery query{m_db};
        if(!query.exec(QStringLiteral("PRAGMA optimize;")))
            qWarning()<<"Failed to run PRAGMA optimize.";
    });
}

DBManager::DBManager() : m_db{}
{
    constexpr int msec = 3 * 60 * 60 * 1000;
    m_timer.start(msec);
    QObject::connect(&m_timer,&QTimer::timeout,QCoreApplication::instance(),[this](){
        QSqlQuery query{m_db};
        if(!query.exec(QStringLiteral("PRAGMA optimize;")))
            qWarning()<<"Failed to run PRAGMA optimize.";
    });
}

DBManager::~DBManager()
{
    if(m_open)
    {
        std::scoped_lock lck{mutex};
        int8_t count = (m_open_connections_count[m_db.connectionName()] -= 1);
        if(count <= 0)
            m_db.close();
    }
}

DBManager::DBManager(const DBManager& other)
{
    m_open = other.m_open.load();
    m_db = other.m_db;
    if(m_open)
    {
        std::scoped_lock lck{mutex};
        m_open_connections_count[m_db.connectionName()] += 1;
    }
}

DBManager& DBManager::operator=(const DBManager& other)
{
    m_open = other.m_open.load();
    m_db = other.m_db;

    if(m_open)
    {
        std::scoped_lock lck{mutex};
        m_open_connections_count[m_db.connectionName()] += 1;
    }
    return *this;
}

bool DBManager::open()
{

    if(!m_db.isValid())
    {
        print_last_db_error(QStringLiteral("Failed to open the database. Error:"));
        return false;
    }
    if(m_db.isOpen())
    {
        if(!m_open)
        {
            std::scoped_lock lck{mutex};
            m_open_connections_count[m_db.connectionName()] += 1;   
            m_open = true;
        }
        return m_open;
    }
    else if(!m_db.open())
    {
        print_last_db_error(QStringLiteral("Failed to open the database. Error:"));
        return false;
    }
    std::scoped_lock lck{mutex};
    m_open_connections_count[m_db.connectionName()] += 1;
    m_open = true;
    return m_open;
}

void DBManager::close()
{
    if(!m_open)
        return;
    mutex.lock();
    int8_t count = (m_open_connections_count[m_db.connectionName()] -= 1);
    mutex.unlock();
    QSqlQuery query(m_db);
    if(!query.exec(QStringLiteral("PRAGMA optimize;")))
        qWarning()<<"Failed to run PRAGMA optimize on database close.";
    //Close only if this is was the last open connection.
    m_open = false;
    if(count <= 0)
        m_db.close();
}

QSqlDriver *DBManager::driver() const
{
    return m_db.driver();
}

void DBManager::set_database_name(const QString &name)
{
    m_db.setDatabaseName(name);
}

bool DBManager::transaction()
{
    return m_db.transaction();
}

bool DBManager::rollback()
{
    return m_db.rollback();
}

bool DBManager::commit()
{
    return m_db.commit();
}

QSqlDatabase& DBManager::get_database()
{
    return m_db;
}

bool DBManager::is_valid() const
{
    return m_db.isValid();
}

bool DBManager::is_open() const
{
    return m_open;
}

QString DBManager::get_database_name() const
{
    return m_db.databaseName();
}

QString DBManager::get_connection_name() const
{
    return m_db.connectionName();
}

int DBManager::table_exists(const QString &table_name) const
{
    if(!m_db.isValid() || !m_db.isOpen())
    {
        qWarning()<<"Can't check the table. Database is valid:"<<m_db.isValid()<<". Database is open:"<<m_db.isOpen();
        return -1;
    }

    QSqlQuery query = m_db.exec(
        QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='%1'").arg(table_name));
    if(!query.isActive())
    {
        qWarning()<<"Query: "<<query.lastQuery()<<"; in DBManager::table_exists() isn't valid. Error:"<<query.lastError();
        return -1;
    }
    if(query.next())
        return 1;
    return 0;
}

//TODO?: make it a template fucntion
void DBManager::print_last_db_error(QStringView text)
{
    qWarning()<<text<<m_db.lastError().text();
}

QString DBManager::journal_mode_to_string(JournalMode mode)
{
    switch(mode)
    {
    case JournalMode::OFF:
        return QStringLiteral("OFF");
    case JournalMode::TRUNCATE:
        return QStringLiteral("TRUNCATE");
    case JournalMode::WAL:
        return QStringLiteral("WAL");
    case JournalMode::MEMORY:
            return QStringLiteral("MEMORY");
    case JournalMode::PERSIST:
            return QStringLiteral("PERSIST");
    case JournalMode::DEL:
            return QStringLiteral("DELETE");
    }
    return {};
}

bool DBManager::connect(const QString &connection_name,bool create_if_not_exist)
{
    if(m_db.connectionName() == connection_name)
        return true;
    if(QSqlDatabase::contains(connection_name))
        m_db = QSqlDatabase::database(connection_name);
    else
        m_db = create_connection(connection_name);

    if(m_db.isValid())
        return true;

    print_last_db_error(QStringLiteral("Database isn't valid. Failed with error:"));
    return false;
}

QSqlDatabase DBManager::create_connection(const QString& connection_name)
{
    if(QSqlDatabase::contains(connection_name))
        return QSqlDatabase::database(connection_name);

    QSqlDatabase db = QSqlDatabase::addDatabase(m_driver_name,connection_name);
    if(db.isValid())
        return db;
    else
    {
        print_last_db_error(QStringLiteral("Database isn't valid. Failed with error:"));
        return QSqlDatabase{};
    }
}

bool DBManager::remove_connection(const QString &connection_name)
{
    if(!QSqlDatabase::contains(connection_name))
    {
        qDebug()<<"Couldn't remove database connection. No such connection name in a list.";
        return false;
    }
    QSqlDatabase::database(connection_name).close();
    QSqlDatabase::removeDatabase(connection_name);
    return true;
}

bool DBManager::wal_checkpoint() const
{
    QVariant v = m_db.driver()->handle();
    if (!v.isValid() && (qstrcmp(v.typeName(), "sqlite3*") == 0)) {
        qWarning()<<"Cannot get a sqlite3 handle to the driver."
                      "Will not be able to complete manual WAL checkpoint.";
        return false;
    }
    sqlite3* handle = *static_cast<sqlite3**>(v.data());
    if (!handle) {
        qWarning()<<"Cannot get a sqlite3 handle in DBManager::wal_checkpoint. "
                      "Wal checkpoint failed.";
        return false;
    }
    int status = sqlite3_wal_checkpoint_v2(handle,"main",SQLITE_CHECKPOINT_FULL,0,0);

    if (status == SQLITE_OK )
        return true;
    else if(status == SQLITE_BUSY)
    {
        qWarning()<<"Couldn't complete wal checkpoint, SQLITE_BUSY returned";
        return false;
    }
    else if(status == SQLITE_ERROR)
    {
        qWarning()<<"Couldn't complete wal checkpoint, returned: SQLITE_ERROR with error:"<<sqlite3_errmsg(handle);
        return false;
    }

    qWarning()<<"Couldn't complete wal checkpoint. Error:"<<sqlite3_errmsg(handle);
    return false;
}

bool DBManager::set_journal_mode(JournalMode mode)
{
    QSqlQuery query(m_db);
    bool res = query.exec(QStringLiteral("PRAGMA journal_mode=") + journal_mode_to_string(mode));
    if(!res)
    {
        qFatal()<<"Couldn't switch db to journal_mode = "<< journal_mode_to_string(mode);
        return false;
    }

    if(mode == JournalMode::WAL)
    {
        QVariant v = m_db.driver()->handle();
        if (!v.isValid() && (qstrcmp(v.typeName(), "sqlite3*") == 0)) {
            qWarning()<<"Cannot get a sqlite3 handle to the driver."
                          "Db will not emit checkpoint signal.";
            return false;
        }
        // Create a handler and attach functions.
        sqlite3* handle = *static_cast<sqlite3**>(v.data());
        if (!handle) {
            qWarning()<<"Cannot get a sqlite3 handle in DBManager::set_journal_mode. "
                          "Db will not emit checkpoint signal.";
            return false;
        }

        sqlite3_wal_hook(handle,
                         [](void*,sqlite3*,const char*,int)->int{
                            emit GlobalSignalEmitter::get_instance()->db_wal_checkpoint();
                            return SQLITE_OK;},
                        nullptr);

    }
    return true;

}

}
