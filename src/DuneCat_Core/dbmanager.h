#pragma once
#include <QtSql>
#include <map>
#include <atomic>
namespace DuneCat
{
class DBManager
{
public:
    DBManager(const QString& connection_name, const QString &database_name = QStringLiteral(""));
    DBManager();
    virtual ~DBManager();
    DBManager(DBManager&& other) = delete;
    DBManager& operator=(DBManager&& other) = delete;
    explicit DBManager(const DBManager& other);
    DBManager& operator=(const DBManager& other);

    bool connect(const QString& connection_name, bool create_if_not_exist = true);
    QSqlDatabase create_connection(const QString& connection_name);
    bool remove_connection(const QString& connection_name);
    bool open();
    void close();

    void set_database_name(const QString& name);
    [[nodiscard]] QString get_database_name() const;
    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] bool is_open() const;
    [[nodiscard]] QSqlDatabase& get_database();
    [[nodiscard]] QString get_connection_name() const;
    //returns -1 in case of error
    [[nodiscard]] int table_exists(const QString& table_name) const;

protected:
    void print_last_db_error(QLatin1StringView text);
private:
    QSqlDatabase m_db;
    bool m_open;
    static std::map<QString,std::atomic_int8_t> m_open_connections_count;
    static const QString m_driver_name;
    std::mutex mutex;
};
}
