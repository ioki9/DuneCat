#include "dcprocesstablemodel.h"

DCProcessTableModel::DCProcessTableModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_proc_tracker = new DCProcessTracker();
    m_processes = m_proc_tracker->get_active_processes();
    connect(m_proc_tracker,&DCProcessTracker::new_process_created,this,&DCProcessTableModel::add_new_process);
}


int DCProcessTableModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return m_processes.size();
}

QVariant DCProcessTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(rowCount() == 0)
        return QVariant();
    switch(role)
    {
    case ProcessNameRole:
        return QVariant(m_processes[index.row()].name);
    case PIDRole:
        return QVariant(m_processes[index.row()].pid);
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

void DCProcessTableModel::add_new_process(const DCProcessInfo& proc)
{
    beginInsertRows(QModelIndex(), rowCount(),rowCount());
    m_processes.append(proc);
    endInsertRows();
}
