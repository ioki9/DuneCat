#include "dcprocesstablemodel.h"

DCProcessTableModel::DCProcessTableModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DCProcessTableModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

bool DCProcessTableModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

bool DCProcessTableModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return false;
}

void DCProcessTableModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
}

QVariant DCProcessTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool DCProcessTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}
