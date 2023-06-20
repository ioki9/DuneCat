#include "dbmanager.h"
#include <span>
namespace DuneCat
{
DBManager::DBManager(const QString& connection_name,const QString& database_name)
{
    m_db = QSqlDatabase::database(connection_name);
    if(!m_db.isValid())
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),connection_name);
    m_db.setDatabaseName(database_name);
    if(!m_db.open())
        qDebug() << m_db.lastError().text();
}

bool DBManager::connection_succeeded() const
{

    return m_connection_succeeded;
}
}
