#pragma once
#include <QSqlQueryModel>
#include <QSqlQuery>
#include "essentialheaders.h"
#include <QAbstractTableModel>
#include "dbmanager.h"
#include "processinfo.h"
#include "sqlqueryfilter.h"

namespace DuneCat
{
class SqlTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SqlTableModel)
    enum SqlRoles
    {
        DisplayDateRole = Qt::UserRole
    };
public:
    explicit SqlTableModel(const DBManager &db, QObject *parent = nullptr);
    ~SqlTableModel() override;
    void setQuery(const QString &query);
    Q_INVOKABLE int columnWidth(int c,int role, const QFont *font = nullptr);
    Q_INVOKABLE int columnWidth(int c,int role, int pointSize);
    Q_INVOKABLE bool refresh(bool isDataChanged = true);
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setFilterText(const QString& filter, const QList<int> &columns, int filterId);
    bool setFilterText(const QString& filter,int column, int filterId);
    bool setFilterDate(const QDateTime &min_date, const QDateTime &max_date, int column, int filterId);
    void sort(int column, Qt::SortOrder order, int filterId);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void setHeaderData(const std::vector<QVariant>& data);
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    qint64 sinceLastModelReset() const;
    bool checkpoint_refresh();
private slots:
    void updateFilters();
private:
    QHash<int, QByteArray> generate_roles_from_fields();
    std::vector<int> m_column_widths;
    DBManager m_db;
    QString m_main_string_query;
    std::unique_ptr<QSqlQuery> m_query;
    QSqlRecord m_record;
    std::vector<QVariant> m_header_data;
    QElapsedTimer m_reset_timer;
    SqlQueryFilter m_filters;
    int m_row_count{0};
    bool m_currently_filtered{false};
};


}
