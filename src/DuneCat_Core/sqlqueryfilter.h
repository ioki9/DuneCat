#pragma once
#include <QObject>
#include <QMap>
#include <optional>
namespace DuneCat
{
class SqlQueryFilter : public QObject
{

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SqlQueryFilter)
public:
    explicit SqlQueryFilter(QObject *parent = nullptr);
    bool add_filter(const QString& filter,int id);
    bool remove_filter(int id);
    bool modify_filter(const QString& new_filter, int id);
    [[nodiscard]] QMap<int,QString> get_filter_list() const;
    [[nodiscard]] QMap<int,QString>* get_filter_list_ptr();
    [[nodiscard]] std::optional<QString> get_filter_by_id(int id) const;
    [[nodiscard]] QString get_filters_string() const;
    [[nodiscard]] bool contains(int id) const;
    [[nodiscard]] int get_filters_amount() const;
signals:
    void filters_changed();
    void _filters_modified();
private:
    QMutex m_mutex{};
    QMap<int,QString> m_filter_list{};
    int m_amount{0};
};
}
