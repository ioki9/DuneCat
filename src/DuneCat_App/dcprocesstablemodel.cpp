#include "dcprocesstablemodel.h"

DCProcessTableModel::DCProcessTableModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_proc_tracker = new DCProcessTracker();
    connect(m_proc_tracker,&DCProcessTracker::begin_process_list_update,
            this,&DCProcessTableModel::beginResetModel);

    connect(m_proc_tracker,&DCProcessTracker::end_process_list_update,
            this,&DCProcessTableModel::endResetModel);
}

int DCProcessTableModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return m_proc_tracker->get_process_count();
}

QVariant DCProcessTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    switch(role)
    {
    case ProcessNameRole:
        return QVariant(m_proc_tracker->get_active_processes()[index.row()].name);
    case PIDRole:
        return QVariant(m_proc_tracker->get_active_processes()[index.row()].process_id);
    }
    return QVariant();
}

bool DCProcessTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();
    return true;
}

QHash<int, QByteArray> DCProcessTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ProcessNameRole] = "name";
    roles[PIDRole] = "PID";
    return roles;
}
