
#include "sqlqueryfilter.h"
namespace DuneCat{

SqlQueryFilter::SqlQueryFilter(QObject *parent)
    : QObject{parent}
{
}

bool SqlQueryFilter::add_filter(const QString &filter, int id)
{
    QMutexLocker locker(&m_mutex);
    if(m_filter_list.contains(id))
    {
        qWarning()<<"Can't add filter to SqlQueryFilter. Filter with provided id:"<<id<<"already exists.";
        return false;
    }
    m_filter_list.insert(id,filter);
    m_amount++;
    emit filters_changed();
    return true;
}

bool SqlQueryFilter::remove_filter(int id)
{
    QMutexLocker locker(&m_mutex);
    if(!m_filter_list.contains(id))
    {
        qWarning()<<"Can't remove filter in SqlQueryFilter. Filter with provided id:"<<id<<"doesn't exist.";
        return false;
    }
    m_filter_list.remove(id);
    m_amount--;
    emit filters_changed();
    return true;
}

bool SqlQueryFilter::modify_filter(const QString &new_filter, int id)
{
    QMutexLocker locker(&m_mutex);
    if(!m_filter_list.contains(id))
    {
        qWarning()<<"Can't modify filter in SqlQueryFilter. Filter with provided id:"<<id<<"doesn't exist.";
        return false;
    }
    if(new_filter.isEmpty())
    {
        qWarning()<<"Used filter in modify_filter of SqlQueryFilter is empty. To delete the filter use remove_filter.";
        return false;
    }
    m_filter_list[id] = new_filter;
    emit filters_changed();
    return true;
}

QMap<int, QString> SqlQueryFilter::get_filter_list() const
{
    return m_filter_list;
}

QMap<int, QString> *SqlQueryFilter::get_filter_list_ptr()
{
    return &m_filter_list;
}

std::optional<QString> SqlQueryFilter::get_filter_by_id(int id) const
{
    if(!m_filter_list.contains(id))
    {
        qWarning()<<"Can't get filter of SqlQueryFilter. Provided id:"<<id<<"doesn't exists.";
        return std::nullopt;
    }
    return {m_filter_list[id]};
}

QString SqlQueryFilter::get_filters_string() const
{
    QString filters{""};
    for(auto i{m_filter_list.begin()}; i != m_filter_list.end(); std::next(i))
    {
        filters += *i;
    }
    return filters;
}

bool SqlQueryFilter::contains(int id) const
{
    return m_filter_list.contains(id);
}

int SqlQueryFilter::get_filters_amount() const
{
    return m_amount;
}


}
