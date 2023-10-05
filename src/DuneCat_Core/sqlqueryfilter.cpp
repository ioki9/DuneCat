
#include "sqlqueryfilter.h"
namespace DuneCat{

SqlQueryFilter::SqlQueryFilter(QObject *parent)
    : QObject{parent}
{

}

bool SqlQueryFilter::add_filter(const QString &filter, int id, FilterType type)
{
    if(m_filter_map.contains(id))
    {
        qWarning()<<"Can't add filter to SqlQueryFilter. Filter with provided id:"<<id<<"already exists.";
        return false;
    }
    m_filter_map.insert(id,{type,filter});
    m_amount++;
    m_dirty = true;
    emit filters_changed();
    return true;
}

bool SqlQueryFilter::remove_filter(int id)
{
    if(!m_filter_map.contains(id))
    {
        qWarning()<<"Can't remove filter in SqlQueryFilter. Filter with provided id:"<<id<<"doesn't exist.";
        return false;
    }
    m_filter_map.remove(id);
    m_amount--;
    m_dirty = true;
    emit filters_changed();
    return true;
}

bool SqlQueryFilter::modify_filter(const QString &new_filter, int id, FilterType new_type)
{
    if(!m_filter_map.contains(id))
    {
        qWarning()<<"Can't modify filter in SqlQueryFilter. Filter with provided id:"<<id<<"doesn't exist.";
        return false;
    }
    if(new_filter.isEmpty())
    {
        qWarning()<<"Used filter in modify_filter of SqlQueryFilter is empty. To delete the filter use remove_filter.";
        return false;
    }
    m_filter_map[id] = {new_type,new_filter};
    m_dirty = true;
    emit filters_changed();
    return true;
}

QMap<int, SqlQueryFilter::Statement> SqlQueryFilter::get_filter_list() const
{
    return m_filter_map;
}

QMap<int, SqlQueryFilter::Statement>* SqlQueryFilter::get_filter_list_ptr()
{
    return &m_filter_map;
}

std::optional<QString> SqlQueryFilter::get_filter_by_id(int id) const
{
    if(!m_filter_map.contains(id))
    {
        qWarning()<<"Can't get filter of SqlQueryFilter. Provided id:"<<id<<"doesn't exists.";
        return std::nullopt;
    }
    return {m_filter_map[id].text};
}

QString SqlQueryFilter::get_filters_string() const
{
    if(!m_dirty)
        return m_filter_string;
    QList<QList<QString>> list(static_cast<int>(FilterType::max_types),{});
    m_filter_string.clear();
    for(auto i{m_filter_map.begin()}; i != m_filter_map.end(); i++)
    {
        list[static_cast<int>(i->type)].append(i->text);
    }
    for(int k{0}; k < list.size(); k++)
    {
        if(list[k].empty())
            continue;

        if(k != 0 || k != list.size() - 1)
            m_filter_string += " ";

        switch(static_cast<FilterType>(k))
        {
            case FilterType::Where:
                m_filter_string+= "WHERE ";
                break;
            case FilterType::OrderBy:
                m_filter_string += "ORDER BY ";
                break;
            case FilterType::GroupBy:
                m_filter_string += "GROUP BY ";
                break;
            case FilterType::Having:
                m_filter_string += "HAVING ";
                break;
            case FilterType::Join:
                m_filter_string += "JOIN ";
                break;
        }

        for(int j{0}; j< list[k].size(); j++)
        {
            m_filter_string += list[k][j];
            if(j+ 1 != list[k].size())
                m_filter_string += " AND ";
        }
    }
    m_dirty = false;
    qDebug()<<m_filter_string;
    return m_filter_string;
}

bool SqlQueryFilter::contains(int id) const
{
    return m_filter_map.contains(id);
}

int SqlQueryFilter::get_filters_amount() const
{
    return m_amount;
}


}
