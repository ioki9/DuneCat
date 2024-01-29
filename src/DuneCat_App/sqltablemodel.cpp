
#include "sqltablemodel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QFontMetrics>

//TODO: SqlQueryFilter -> specific functions to create filter string from different values
//      addFilter(...,filterId), removeFilter(filterId), modifyFilter(...,filterId) functions to work with current filters.
//      Use filter array to construct end point query.
namespace DuneCat
{
SqlTableModel::SqlTableModel(const DBManager& db,QObject *parent)
    : QAbstractTableModel{parent},m_db{db},m_filters{this}
{

    m_reset_timer.start();
    if(!m_db.is_open())
        m_db.open();
    m_query = std::make_unique<QSqlQuery>(m_db.get_database());
    QSqlQuery count_query(m_db.get_database());
    count_query.exec(QStringLiteral("SELECT COUNT(*) FROM processes_history"));
    if(count_query.next())
        m_row_count = count_query.value(0).toInt();
    else
        qWarning()<<"can't get row count of sql database. Error: "<<count_query.lastError().text();
}

SqlTableModel::~SqlTableModel() = default;

void SqlTableModel::setQuery(const QString &query)
{
    if(!m_query->prepare(query))
    {
        qWarning()<<"Cloudn't prepare query in SqlTableModel. Error:"<<m_query->lastError().text()
                   <<". Query:"<<m_query->lastQuery();
        return;
    }
    m_main_string_query = query;
    if(m_query->exec())
        m_record = m_query->record();
    else
        qWarning()<<"Couldn't execute query in SqlTableModel. Error:"<<m_query->lastError().text()
            <<". Query:"<<m_query->lastQuery();

    m_column_widths = std::vector<int>(this->columnCount(QModelIndex()),0);
}

int SqlTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return m_record.count();
}

QVariant SqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    for(size_t i{0}; i < m_header_data.size();i++)
        if(i == section)
            return m_header_data.at(i);

    return {};
}

void SqlTableModel::setHeaderData(const std::vector<QVariant> &data)
{
    if(!data.empty())
        m_header_data = data;
}

int SqlTableModel::rowCount(const QModelIndex &parent) const
{
    return m_row_count;
}

bool SqlTableModel::setFilterText(const QString& filter, const QList<int> &columns,int filterId)
{
    if(filter.isEmpty())
    {
        m_filters.remove_filter(filterId);
        this->updateFilters();
        return this->refresh(false);
    }
    QString filter_query{"("};
    for(size_t i{0};i < columns.size() - 1; i++)
        filter_query += m_record.fieldName(columns[i]) + QStringLiteral(" LIKE '%") + filter + "%' OR ";

    filter_query += m_record.fieldName(columns.back()) + QStringLiteral(" LIKE '%") + filter + "%')";
    if(!m_filters.contains(filterId))
        m_filters.add_filter(filter_query,filterId,FilterType::Where);
    else
        m_filters.modify_filter(filter_query,filterId,FilterType::Where);

    this->updateFilters();
    return this->refresh(false);
}

bool SqlTableModel::setFilterText(const QString& filter, int column,int filterId)
{
    if(filter.isEmpty())
    {
        m_filters.remove_filter(filterId);
        this->updateFilters();
        return this->refresh(false);
    }
    QString filter_query{"("};
    filter_query += m_record.fieldName(column) + QStringLiteral(" LIKE '%") + filter + "%')";
    if(!m_filters.contains(filterId))
        m_filters.add_filter(filter_query,filterId,FilterType::Where);
    else
        m_filters.modify_filter(filter_query,filterId,FilterType::Where);
    this->updateFilters();
    return this->refresh(false);

}

bool SqlTableModel::setFilterDate(const QDateTime &min_date,const QDateTime &max_date,int column,int filterId)
{
    if(!min_date.isValid() || !max_date.isValid())
    {
        qWarning()<<"Dates in setFilterDate that are provided for filtering isn't valid.";
        return false;
    }

    auto filter = QString("(%1 BETWEEN %2 AND %3)")
                  .arg(m_record.fieldName(column),QString::number(min_date.toSecsSinceEpoch()),QString::number(max_date.toSecsSinceEpoch()));
    if(!m_filters.contains(filterId))

        m_filters.add_filter(filter,filterId,FilterType::Where);
    else
        m_filters.modify_filter(filter,filterId,FilterType::Where);
    this->updateFilters();

    return this->refresh(false);
}

void SqlTableModel::sort(int column, Qt::SortOrder order, int filterId)
{
    static int last_filter_id = filterId;
    QString filter{""};
    filter += m_record.fieldName(column) + " ";
    filter += order == Qt::AscendingOrder ? "ASC" : "DESC";
    if(m_filters.contains(filterId))
        m_filters.modify_filter(filter,filterId,FilterType::OrderBy);
    else
    {
        m_filters.remove_filter(last_filter_id);
        last_filter_id  = filterId;
        m_filters.add_filter(filter,filterId,FilterType::OrderBy);
    }
    this->updateFilters();
}

QHash<int, QByteArray> SqlTableModel::generate_roles_from_fields()
{

    QHash<int, QByteArray> roles;
    for( int i = 0; i < m_record.count(); i ++) {
        roles.insert(Qt::UserRole + i + 1, m_record.fieldName(i).toUtf8());
    }
    return roles;
}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
    if(m_query->seek(index.row()))
    {
        if(role != DisplayDateRole)
            return m_query->value(index.column());

        int ts = m_query->value(index.column()).toInt();
        if(!ts)
            return {"running"};
        QString date_str{""};
        if(ts < 0)
        {
            ts *= -1;
            date_str += QStringLiteral("After ");
        }
        QDateTime date = QDateTime::fromSecsSinceEpoch(ts).toLocalTime();
        date_str += QLocale::system().toString(date,QLocale::ShortFormat);

        return {date_str};
    }
    return {""};
}


QHash<int, QByteArray> SqlTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[DisplayDateRole] = "displayDate";
    return roles;
}

qint64 SqlTableModel::sinceLastModelReset() const
{
    return m_reset_timer.elapsed();
}

bool SqlTableModel::checkpoint_refresh()
{
    m_query->finish();
    if(!m_db.wal_checkpoint())
        return false;
    return refresh(true);
}

void SqlTableModel::updateFilters()
{
    const int amount = m_filters.get_filters_amount();
    if(!amount)
    {
        m_query->prepare(m_main_string_query);
        this->refresh();
    }
    else
    {
        auto res = m_main_string_query + QStringLiteral(" ") + m_filters.get_filters_string();
        m_query->prepare(res);
        this->refresh();
    }
}

int SqlTableModel::columnWidth(int c,int role, const QFont *font)
{
    if(!m_column_widths[c])
    {
        QFontMetrics default_metrics = QFontMetrics(QApplication::font());
        QFontMetrics fm = (font ? QFontMetrics(*font) : default_metrics);
        int ret = fm.horizontalAdvance(headerData(c,Qt::Horizontal,role).toString()
                                       + QStringLiteral(" \u2B9F")) + 10;

        int rows = qMax(m_row_count,3000);
        for (int r{0};r<rows;++r)
            ret = qMax(ret,(fm.horizontalAdvance(data(QAbstractItemModel::createIndex(r,c), role).toString())+6));

        m_column_widths[c] = ret;
    }
    return m_column_widths[c];
}

int SqlTableModel::columnWidth(int c,int role, int pointSize)
{
    if(!m_column_widths[c])
    {
        auto* font = new QFont(QGuiApplication::font());
        font->setPointSize(pointSize);
        columnWidth(c,role, font);
    }
    return m_column_widths[c];
}

bool SqlTableModel::refresh(bool isDataChanged)
{
    QSqlQuery count_query(m_db.get_database());
    count_query.exec(QStringLiteral("SELECT COUNT(*) FROM processes_history"));
    if(count_query.next())
    {
        int rows{count_query.value(0).toInt()};
        if(m_row_count > rows)
        {

            beginRemoveRows({},rows,m_row_count);
            m_row_count = rows;
            endRemoveRows();
        }
        else if(m_row_count < rows)
        {
            beginInsertRows({},m_row_count,rows);
            m_row_count = rows;
            endInsertRows();
        }
    }
    else
    {
        qWarning()<<"can't get row count of sql database. Error: "<<count_query.lastError().text();
        return false;
    }
    count_query.finish();
    if(!m_query->exec())
    {
        qWarning()<<"Couldn't refresh SqlTableModel. Query exec error. Error:"<<m_query->lastError().text()
                   <<". Query:"<<m_query->lastQuery();
        return false;
    }
    if(isDataChanged)
        emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    m_reset_timer.restart();

    return true;
}

}
