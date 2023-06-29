#include "dbmanager.h"

namespace DuneCat
{
std::map<QString,std::atomic_int8_t> DBManager::m_open_connections_count{};
QString DBManager::m_driver_name = QStringLiteral("QSQLITE");

DBManager::DBManager(const QString& connection_name,const QString& database_name)
{
    if(QSqlDatabase::contains(connection_name))
    {
        m_db = QSqlDatabase::database(connection_name);
        return;
    }
    m_db = QSqlDatabase::addDatabase(m_driver_name,connection_name);
    if(!m_db.isValid())
    {
        print_last_db_error(QLatin1StringView("Database isn't valid. Failed with error:"));
            return;
    }

    if(!database_name.isEmpty())
        m_db.setDatabaseName(database_name);
}

DBManager::DBManager()
{

}

DBManager::~DBManager()
{
    int8_t count = (m_open_connections_count[m_db.connectionName()] -= 1);
    if(m_db.isOpen() && count <= 0)
        m_db.close();
}

DBManager::DBManager(DBManager&& other)
{
    m_db = QSqlDatabase::database(other.m_db.connectionName(),other.m_db.isOpen());
    other.m_db.close();
    if(m_db.isValid())
    {
        m_db.setDatabaseName(other.m_db.databaseName());
    }
    else
        print_last_db_error(QLatin1StringView("Moved database isn't valid. Failed with error:"));
}

DBManager& DBManager::operator=(DBManager&& other)
{
    if(m_db.isOpen())
        m_db.close();
    m_db = QSqlDatabase::database(other.m_db.connectionName(),other.m_db.isOpen());

    if(m_db.isValid())
    {
        m_db.setDatabaseName(other.m_db.databaseName());
    }
    else
        print_last_db_error(QLatin1StringView("Moved database isn't valid. Failed with error:"));

    return *this;
}



bool DBManager::open()
{

    if(!m_db.isValid())
    {
        print_last_db_error(QLatin1StringView("Failed to open the database. Error:"));
        return false;
    }
    if(m_db.isOpen())
        return true;
    else if(!m_db.open())
    {
        print_last_db_error(QLatin1StringView("Failed to open the database. Error:"));
        return false;
    }
    //Counting every reference to the connection
    m_connections_count[m_db.connectionName()] += 1;
    return true;
}

bool DBManager::open(const QString& database_name)
{

    if(!m_db.isValid())
    {
        print_last_db_error(QLatin1StringView("Failed to open the database. Error:"));
        return false;
    }
    if(m_db.isOpen())
        m_db.close();
    m_db.setDatabaseName(database_name);
    if(!m_db.isValid() || !m_db.open())
    {
        print_last_db_error(QLatin1StringView("Failed to open the database. Error:"));
        return false;
    }
    //Counting every reference to the connection
    m_connections_count[m_db.connectionName()] += 1;
    return true;
}

void DBManager::close()
{

    int8_t count = (m_open_connections_count[m_db.connectionName()] -= 1);
    //Close only if this is was the last open connection.
    if(count <= 0)
        m_db.close();
}

void DBManager::set_database_name(const QString &name)
{
    m_db.setDatabaseName(name);
}

QSqlDatabase &DBManager::get_database() const
{
    return m_db;
}

bool DBManager::is_valid() const
{
    return m_db.isValid();
}

bool DBManager::is_open() const
{
    return m_db.isOpen();
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
        qWarning()<<"Can't check table. Database is valid:"<<m_db.isValid()<<". Database is open:"<<m_db.isOpen();
        return -1;
    }

    QSqlQuery query = m_db.exec(
        QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='%1'").arg(table_name));
    if(!query.isActive())
    {
        qWarning()<<"Query to check the table isn't valid. Error:"<<query.lastError();
        return -1;
    }
    if(query.next())
        return 1;
    return 0;
}

//TODO?: make it a template fucntion
void DBManager::print_last_db_error(QLatin1StringView text)
{
    qWarning()<<text<<m_db.lastError().text();
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

    print_last_db_error(QLatin1StringView("Database isn't valid. Failed with error:"));
    return false;
}

QSqlDatabase DBManager::create_connection(const QString& connection_name)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains(connection_name))
        return db = QSqlDatabase::database(connection_name);

    db = QSqlDatabase::addDatabase(m_driver_name,connection_name);
    if(db.isValid())
        return db;
    else
    {
        print_last_db_error(QLatin1StringView("Database isn't valid. Failed with error:"));
        return QSqlDatabase{};
    }
}

bool DBManager::remove_connection(const QString &connection_name)
{
    if(!QSqlDatabase.contains(connection_name))
    {
        qDebug()<<"Couldn't remove database connection. No such connection name in a list.";
        return false;
    }
    QSqlDatabase::database(connection_name).close();
    QSqlDatabase::removeDatabase(connection_name);
    return true;
}

}
