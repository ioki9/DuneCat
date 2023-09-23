#include "sqlsortfiltermodel.h"
#include <QSqlQuery>
namespace DuneCat
{
SqlSortFilterModel::SqlSortFilterModel(QSqlDatabase db,QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

void SqlSortFilterModel::sort(int column, Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(column,order);
}

int SqlSortFilterModel::rowCount(const QModelIndex &parent) const
{
    return m_model->rowCount(parent);
}

int SqlSortFilterModel::columnCount(const QModelIndex &parent) const
{
    return m_model->columnCount(QModelIndex());
}

int SqlSortFilterModel::columnWidth(int c,int role, const QFont *font)
{
    if(c < 0|| c >= m_model->columnCount(QModelIndex()))
        return 0;
    return m_model->columnWidth(c,role,font);
}

int SqlSortFilterModel::columnWidth(int c,int role, int pointSize)
{
    if(c < 0|| c >= m_model->columnCount(QModelIndex()))
        return 0;
    return m_model->columnWidth(c,role,pointSize);
}

QVariant SqlSortFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model->headerData(section,orientation,role);
}

void SqlSortFilterModel::refresh()
{
    m_model->checkpoint_refresh();
}

void SqlSortFilterModel::setFilterText(const QString &pattern,int column,int filterId)
{
    m_model->setFilterText(pattern,column,filterId);
}

void SqlSortFilterModel::setFilterText(const QString &pattern, const QList<int> &columns,int filterId)
{
    m_model->setFilterText(pattern,columns,filterId);

}

void SqlSortFilterModel::setFilterDate(const QDateTime &min_date,const QDateTime &max_date, int column, int filterId)
{
    m_model->setFilterDate(min_date,max_date,column,filterId);
}

void SqlSortFilterModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_model = qobject_cast<SqlTableModel*>(sourceModel);
    if(!m_model)
        qWarning()<<"dynamic_cast of QAbstractItemModel to SqlTableModel failed in SqlSortFilterModel::setSourceModel.";
    QSortFilterProxyModel::setSourceModel(sourceModel);
}
}
