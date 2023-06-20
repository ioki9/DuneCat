#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql>


namespace DuneCat
{
class DBManager
{
public:
    DBManager(const QString& connection_name, const QString &database_name);
    [[no_discard]] bool connection_succeeded() const;
private:
    QSqlDatabase m_db;
    bool m_connection_succeeded{false};
};
}
#endif // DBMANAGER_H
