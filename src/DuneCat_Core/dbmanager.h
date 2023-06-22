#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql>


namespace DuneCat
{
class DBManager
{
public:
    DBManager(const QString& connection_name, const QString &database_name);
    ~DBManager();
    DBManager(DBManager&& other);
    DBManager& operator=(DBManager&& other);
    DBManager(const DBManager& copy)= delete;
    DBManager& operator=(const DBManager& copy) = delete;
    bool open_connection();
    void close_connection();
    [[nodiscard]] inline bool is_valid() const {return m_is_valid;}
    //TODO: add function to try adding a database again.
private:
    QSqlDatabase m_db;
    bool m_is_valid{false};
    QString m_connection_name;
    QString m_db_name;
};
}
#endif // DBMANAGER_H
