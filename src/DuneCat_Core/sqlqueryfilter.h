#pragma once
#include <QObject>
#include <QMap>
#include <optional>
namespace DuneCat
{

enum class FilterType : short
{
    Where,
    GroupBy,
    Having,
    Join,
    OrderBy,
    max_types
};

class SqlQueryFilter : public QObject
{

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SqlQueryFilter)
public:

    struct Statement
    {
        FilterType type;
        QString text;
    };

    explicit SqlQueryFilter(QObject *parent = nullptr);
    bool add_filter(const QString& filter, int id, FilterType type);
    bool remove_filter(int id);
    bool modify_filter(const QString& new_filter, int id, FilterType new_type);
    [[nodiscard]] QMap<int,Statement> get_filter_list() const;
    [[nodiscard]] QMap<int,Statement>* get_filter_list_ptr();
    [[nodiscard]] std::optional<QString> get_filter_by_id(int id) const;
    [[nodiscard]] QString get_filters_string() const;
    [[nodiscard]] bool contains(int id) const;
    [[nodiscard]] int get_filters_amount() const;
signals:
    void filters_changed();
    void _filters_modified();
private:
    QMap<int,Statement> m_filter_map;
    mutable bool m_dirty{true};
    mutable QString m_filter_string{};
    int m_amount{0};
};
}
