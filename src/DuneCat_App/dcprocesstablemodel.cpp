#include "dcprocesstablemodel.h"

DCProcessTableModel::DCProcessTableModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_proc_tracker = new DCProcessTracker(this);
    m_processes = m_proc_tracker->get_active_processes();
    connect(m_proc_tracker,&DCProcessTracker::process_created,this,&DCProcessTableModel::add_new_process);
    connect(m_proc_tracker,&DCProcessTracker::process_deleted,this,&DCProcessTableModel::remove_process);
}


int DCProcessTableModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return m_processes.size();
}

QVariant DCProcessTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == ProcessNameRole || section == 1)
        return QVariant(QString("Name"));

    if(role == FilePathRole || section == 0)
        return QVariant(QString("Path"));

    if(role == TerminationDateRole)
        return QVariant(QString("Termination date"));

    if(role == CreationDateRole)
        return QVariant(QString("Creation date"));

    if(role == OwnerUserRole)
        return QVariant(QString("User name"));

    if(role == OwnerDomainRole)
        return QVariant(QString("Domain name"));

    if(role == PIDRole)
        return QVariant(QString("PID"));

    if(role == CommandLineRole)
        return QVariant(QString("Command line args"));

    return QVariant(QString("LULW"));
}


QVariant DCProcessTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(!rowCount())
        return QVariant();
    switch(role)
    {
    case ProcessNameRole:
        return QVariant(m_processes[index.row()].name);
    case PIDRole:
        return QVariant(m_processes[index.row()].pid);
    case TerminationDateRole:
        return QVariant(m_processes[index.row()].termination_date);
    case CreationDateRole:
        return QVariant(m_processes[index.row()].creation_date);
    case FilePathRole:
        return QVariant(m_processes[index.row()].file_path);
    case OwnerUserRole:
        return QVariant(m_processes[index.row()].owner_user);
    case OwnerDomainRole:
        return QVariant(m_processes[index.row()].owner_domain);
    case CommandLineRole:
        return QVariant(m_processes[index.row()].command_line);
    }
    return QVariant();
}

QHash<int, QByteArray> DCProcessTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ProcessNameRole] = "name";
    roles[PIDRole] = "PID";
    roles[TerminationDateRole] = "TerminationDate";
    roles[CreationDateRole] = "CreationDate";
    roles[FilePathRole] = "ExePath";
    roles[OwnerUserRole] = "OwnerUser";
    roles[OwnerDomainRole] = "OwnerDomain";
    roles[CommandLineRole] = "CommandLine";
    return roles;
}

void DCProcessTableModel::add_new_process(const DCProcessInfo& proc)
{
    mutex.lock();
    beginInsertRows(QModelIndex(), rowCount(),rowCount());
    m_processes.push_back(proc);
    endInsertRows();
    mutex.unlock();
}

void DCProcessTableModel::remove_process(const DCProcessInfo& proc)
{
    auto it = std::find_if(m_processes.begin(),m_processes.end(),
                        [&proc](const DCProcessInfo& srch){return srch.pid == proc.pid;});
    if(it == std::end(m_processes))
        return;

    mutex.lock();
    int row = it - m_processes.begin();
    beginRemoveRows(QModelIndex(), row,row);
    m_processes.erase(it);
    endRemoveRows();
    mutex.unlock();
}

