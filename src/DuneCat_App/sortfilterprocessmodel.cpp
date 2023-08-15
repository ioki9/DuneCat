
#include "sortfilterprocessmodel.h"
#include <chrono>

namespace DuneCat
{
SortFilterProcessModel::SortFilterProcessModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setSourceModel(&m_model);
}

void SortFilterProcessModel::sort(int column, Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(column,order);
}

int SortFilterProcessModel::columnCount(const QModelIndex &parent) const
{
    return m_model.columnCount(QModelIndex());
}

int SortFilterProcessModel::columnWidth(int c, const QFont *font)
{
    if(c < 0|| c >= m_model.columnCount(QModelIndex()))
        return 0;
    return m_model.columnWidth(c,font);
}

int SortFilterProcessModel::columnWidth(int c, int pointSize)
{
    if(c < 0|| c >= m_model.columnCount(QModelIndex()))
        return 0;
    return m_model.columnWidth(c,pointSize);
}

QVariant SortFilterProcessModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_model.headerData(section,orientation,role);
}

void SortFilterProcessModel::refresh()
{

}
}
