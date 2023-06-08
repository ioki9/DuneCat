#include "ProcessTableModel.h"
#include <QFont>
#include <QFontMetrics>

namespace DuneCat
{
ProcessTableModel::ProcessTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_column_widths = std::vector<int>(m_column_count,0);
    m_proc_tracker = new ProcessTracker(this);
    m_processes = m_proc_tracker->get_process_list();
    connect(m_proc_tracker,&ProcessTracker::process_created,this,&ProcessTableModel::add_new_process);
    connect(m_proc_tracker,&ProcessTracker::process_deleted,this,&ProcessTableModel::remove_process);
}


int ProcessTableModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    return m_processes.size();
}
int ProcessTableModel::columnCount(const QModelIndex &parent) const
{
    return m_column_count;
}

QVariant ProcessTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    switch(section)
    {
    case static_cast<int>(EColumns::process_name):
        return QString("Process name");
    case static_cast<int>(EColumns::description):
        return QString("Name");
    case static_cast<int>(EColumns::pid):
        return QVariant(QString("PID"));
    case static_cast<int>(EColumns::user_name):
        return QVariant(QString("User name"));
    case static_cast<int>(EColumns::domain_name):
        return QVariant(QString("Domain name"));
    }
    return QVariant();
}

int ProcessTableModel::columnWidth(int c, const QFont *font)
{
    if(!m_column_widths[c])
    {
        if(font == nullptr)
            qDebug()<<"nullptr";
        QFontMetrics default_metrics = QFontMetrics(QGuiApplication::font());
        QFontMetrics fm = (font ? QFontMetrics(*font) : default_metrics);
        int ret = fm.horizontalAdvance(headerData(c,Qt::Horizontal,Qt::DisplayRole).toString()
                                       + QLatin1String(" ^")) + 10;

        for (int r{0};r<m_processes.size();++r)
            ret = qMax(ret,(fm.horizontalAdvance(data(QAbstractItemModel::createIndex(r,c), Qt::DisplayRole).toString())+6));

        m_column_widths[c] = ret;
    }
    return m_column_widths[c];
}

int ProcessTableModel::columnWidth(int c, int pointSize)
{
    if(!m_column_widths[c])
    {
        QFont* font = new QFont(QGuiApplication::font());
        font->setPointSize(pointSize);
        columnWidth(c,font);
    }
    return m_column_widths[c];
}


QVariant ProcessTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !rowCount() || role != Qt::DisplayRole)
        return QVariant();

    switch(index.column())
    {
    case static_cast<int>(EColumns::process_name):
        return QVariant(m_processes[index.row()].name);
    case static_cast<int>(EColumns::description):
        return QVariant(m_processes[index.row()].description);
    case static_cast<int>(EColumns::pid):
        return QVariant(m_processes[index.row()].pid);
    case static_cast<int>(EColumns::user_name):
        return QVariant(m_processes[index.row()].owner_user);
    case static_cast<int>(EColumns::domain_name):
        return QVariant(m_processes[index.row()].owner_domain);
    }
    return QVariant();
}

QHash<int, QByteArray> ProcessTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    return roles;
}


void ProcessTableModel::add_new_process(const ProcessInfo& proc)
{
    mutex.lock();
    beginInsertRows(QModelIndex(), rowCount(),rowCount());
    m_processes.push_back(proc);
    endInsertRows();
    mutex.unlock();
}

void ProcessTableModel::remove_process(const ProcessInfo& proc)
{
    auto it = std::find_if(m_processes.begin(),m_processes.end(),
                           [&proc](const ProcessInfo& srch){return srch.pid == proc.pid;});
    if(it == std::end(m_processes))
        return;

    mutex.lock();
    int row = it - m_processes.begin();
    beginRemoveRows(QModelIndex(), row,row);
    m_processes.erase(it);
    endRemoveRows();
    mutex.unlock();
}

}
