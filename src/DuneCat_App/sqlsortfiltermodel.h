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
    Q_INVOKABLE void sort(int column,Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnWidth(int c,const QFont *font = nullptr);
    Q_INVOKABLE int columnWidth(int c, int pointSize);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void setSourceModel(QAbstractItemModel *sourceModel) override;
private:
    SqlTableModel* m_model;
    QSqlDatabase m_db;
    mutable QSqlQuery m_query;
};

}
