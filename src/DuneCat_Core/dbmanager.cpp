#include "dbmanager.h"

namespace DuneCat
{
DBManager::DBManager(const QString& connection_name,const QString& database_name)
    : m_connection_name{connection_name}, m_db_name{database_name},
    m_driver_name{QStringLiteral("QSQLITE")}
{
    m_db = QSqlDatabase::database(connection_name);
    if(!m_db.isValid())
        m_db = QSqlDatabase::addDatabase(m_driver_name,connection_name);
    if(!m_db.isValid())
    {
        print_last_db_error(QLatin1StringView("Database isn't valid. Failed with error:"));
        m_is_valid = false;
        return;
    }
    if(!database_name.isEmpty())
        m_db.setDatabaseName(database_name);
    m_is_valid = true;
}

DBManager::DBManager()
    : m_connection_name{QStringLiteral("")}, m_db_name{QStringLiteral("")},
    m_driver_name{QStringLiteral("QSQLITE")}
{

}

DBManager::~DBManager()
{
    if(m_db.isOpen())
        m_db.close();
}

DBManager::DBManager(DBManager&& other)
    : m_connection_name{std::move(other.m_connection_name)},m_db_name{std::move(other.m_db_name)}
{
    m_db = QSqlDatabase::database(m_connection_name,other.m_db.isOpen());
    other.m_db.close();
    if(m_db.isValid())
    {
        m_is_valid = true;
        m_db.setDatabaseName(m_db_name);
    }
    else
        print_last_db_error(QLatin1StringView("Moved database isn't valid. Failed with error:"));
}

DBManager& DBManager::operator=(DBManager&& other)
{
    if(m_db.isOpen())
        m_db.close();
    m_db = QSqlDatabase::database(other.m_connection_name,other.m_db.isOpen());

    if(m_db.isValid())
    {
        m_is_valid = true;
        m_db.setDatabaseName(other.m_db_name);
    }
    else
        print_last_db_error(QLatin1StringView("Moved database isn't valid. Failed with error:"));

    m_connection_name = std::move(other.m_connection_name);
    m_db_name = std::move(other.m_db_name);
    return *this;
}
bool DBManager::open_connection()
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
    return true;
}

bool DBManager::open_connection(const QString& database_name)
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
    return true;
}

void DBManager::close_connection()
{
    m_db.close();
}

bool DBManager::create_connection(const QString& connection_name,bool open)
{
    if(m_db.isValid())
    {
        m_is_valid = true;
        if(m_db.isOpen() == open)
            return true;

        if(open)
            return open_connection();
        else
            m_db.close();

        return true;
    }

    m_db = QSqlDatabase::database(connection_name,open);
    if(m_db.isValid())
        return m_is_valid = true;;

    m_db = QSqlDatabase::addDatabase(m_driver_name,connection_name);
    if(m_db.isValid())
    {
        m_is_valid = true;
        if(open)
            return open_connection();
        return true;
    }
    else
    {
        print_last_db_error(QLatin1StringView("Database isn't valid. Failed with error:"));
        return m_is_valid = false;
    }
}




}
