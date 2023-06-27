#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QtSql>


namespace DuneCat
{
class DBManager
{
public:
    DBManager(const QString& connection_name, const QString &database_name = QStringLiteral(""));
    DBManager();
    ~DBManager();
    DBManager(DBManager&& other);
    DBManager& operator=(DBManager&& other);
    DBManager(const DBManager& copy)= delete;
    DBManager& operator=(const DBManager& copy) = delete;
    bool create_connection(const QString& connection_name, bool open);
    void remove_connection(const QString& connection_name);
    bool open_connection();
    bool open_connection(const QString& database_name);

    void close_connection();
    inline void set_database_name(const QString& name){m_db_name = name; m_db.setDatabaseName(name);}
    inline void set_driver_name(const QString& driver = QStringLiteral("QSQLITE")){m_driver_name = driver;}
    [[nodiscard]] inline bool is_valid() const {return m_is_valid;}
    [[nodiscard]] inline QString get_database_name() const { return m_db_name;}
    [[nodiscard]] inline QString get_connection_name() const {return m_connection_name;}
    //returns -1 in case of error
    [[nodiscard]] int table_exists(const QString& table_name) const;

private:
    void print_last_db_error(QLatin1StringView text);
    QSqlDatabase m_db;
    bool m_is_valid{false};
    QString m_connection_name;
    QString m_db_name;
    QString m_driver_name;
};
}
#endif // DBMANAGER_H
