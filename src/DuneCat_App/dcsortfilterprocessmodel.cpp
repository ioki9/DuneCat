
#include "dcsortfilterprocessmodel.h"
#include <chrono>
DCSortFilterProcessModel::DCSortFilterProcessModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setSourceModel(&m_model);

}

void DCSortFilterProcessModel::sort(int column, Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(column,order);
}

int DCSortFilterProcessModel::columnCount(const QModelIndex &parent) const
{
    return m_model.columnCount(QModelIndex());
}

int DCSortFilterProcessModel::columnWidth(int c, const QFont *font)
{
    if(c < 0|| c >= m_model.columnCount(QModelIndex()))
        return 0;
    return m_model.columnWidth(c,font);
}

int DCSortFilterProcessModel::columnWidth(int c, int pointSize)
{
    if(c < 0|| c >= m_model.columnCount(QModelIndex()))
        return 0;
    return m_model.columnWidth(c,pointSize);
}

QVariant DCSortFilterProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model.headerData(section,orientation,role);
}

