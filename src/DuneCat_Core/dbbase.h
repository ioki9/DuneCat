#pragma once
#include <QtSql>


namespace DuneCat
{
class DBBase
{
public:
    DBBase(const QString& connection_name, const QString &database_name = QStringLiteral(""));
    DBBase();
    ~DBBase();
    explicit DBBase(DBBase&& other);
    DBBase& operator=(DBBase&& other);
    DBBase(const DBBase& copy)= delete;
    DBBase& operator=(const DBBase& copy) = delete;
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
