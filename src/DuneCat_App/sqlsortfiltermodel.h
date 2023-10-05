#pragma once
#include <QSortFilterProxyModel>
#include <QSql>
#include <QSqlQuery>
#include "sqltablemodel.h"

namespace DuneCat{
class SqlSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SqlSortFilterModel(QSqlDatabase db, QObject *parent = nullptr);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order, int filterId);
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnWidth(int c, int role, const QFont *font = nullptr);
    Q_INVOKABLE int columnWidth(int c, int role, int pointSize);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setFilterText(const QString& pattern, int column, int filterId);
    Q_INVOKABLE void setFilterText(const QString& pattern, const QList<int>& columns,int filterId);
    Q_INVOKABLE void setFilterDate(const QDateTime &min_date, const QDateTime &max_date, int column, int filterId);
    void setSourceModel(QAbstractItemModel *sourceModel) override;
private:
    SqlTableModel* m_model;
};

}
