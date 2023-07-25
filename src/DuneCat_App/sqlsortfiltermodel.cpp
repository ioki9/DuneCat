#include "sqlsortfiltermodel.h"
#include <QSqlQuery>
namespace DuneCat
{
SqlSortFilterModel::SqlSortFilterModel(QSqlDatabase db,QObject *parent)
    : QSortFilterProxyModel{parent}, m_db{db}
{
    m_query.setForwardOnly(true);

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
    m_model->refresh();
}

void SqlSortFilterModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_model = dynamic_cast<SqlTableModel*>(sourceModel);
    if(!m_model)
        qWarning()<<"dynamic_cast of QAbstractItemModel to SqlTableModel failed in SqlSortFilterModel::setSourceModel.";
    QSortFilterProxyModel::setSourceModel(sourceModel);
}
}
