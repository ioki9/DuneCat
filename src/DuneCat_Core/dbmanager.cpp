#include "dbmanager.h"
#include <span>
namespace DuneCat
{
DBManager::DBManager(const QString& connection_name,const QString& database_name) :
    m_connection_name{connection_name}, m_db_name{database_name}
{
    m_db = QSqlDatabase::database(connection_name);
    if(!m_db.isValid())
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),connection_name);
    if(!m_db.isValid())
    {
        qDebug() << "Database isn't valid. Failed with error:"<<m_db.lastError().text();
        m_is_valid = false;
        return;
    }
    m_db.setDatabaseName(database_name);
    m_is_valid = true;
}


DBManager::DBManager(DBManager&& other)
    : m_connection_name{std::move(other.m_connection_name)},m_db_name{std::move(other.m_db_name)}
{
    qDebug()<<"invoke move op";
    m_db = QSqlDatabase::database(m_connection_name,other.m_db.isOpen());
    if(m_db.isValid())
    {
        m_is_valid = true;
        m_db.setDatabaseName(m_db_name);
    }
    else
        qDebug() <<"Moved database isn't valid. Failed with error:"<< m_db.lastError().text();

    if(other.m_db.isOpen())
        other.m_db.close();

}

DBManager& DBManager::operator=(DBManager&& other)
{
    qDebug()<<"invoke = op";
    if(m_db.isOpen())
        m_db.close();
    m_db = QSqlDatabase::database(other.m_connection_name,other.m_db.isOpen());
    if(other.m_db.isOpen())
        other.m_db.close();

    if(m_db.isValid())
    {
        m_is_valid = true;
        m_db.setDatabaseName(other.m_db_name);
    }
    else
        qDebug() <<"Moved database isn't valid. Failed with error:"<< m_db.lastError().text();
    m_connection_name = std::move(other.m_connection_name);
    m_db_name = std::move(other.m_db_name);
    return *this;
}
bool DBManager::open_connection()
{
    if(!m_db.isValid() || !m_db.open())
    {
        qDebug()<<"failed to open the database. Error:"<<m_db.lastError().text();
        return false;
    }
    return true;
}

void DBManager::close_connection()
{
    m_db.close();
}

DBManager::~DBManager()
{
    if(m_db.isOpen())
        m_db.close();
}



}
