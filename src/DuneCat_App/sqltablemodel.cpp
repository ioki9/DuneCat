
#include "sqltablemodel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QFontMetrics>
#include "processtracker.h"
namespace DuneCat
{
SqlTableModel::SqlTableModel(const DBManager& db,QObject *parent)
    : QAbstractTableModel{parent},m_db{db}
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
    connect(this,&SqlTableModel::rwoChanged,this,&SqlTableModel::rowChangedHandler);
}

SqlTableModel::~SqlTableModel()
{
}

void SqlTableModel::setQuery(const QString &query)
{
    if(!m_query->prepare(query))
    {
        qWarning()<<"Cloudn't prepare query in SqlTableModel. Error:"<<m_query->lastError().text()
                   <<". Query:"<<m_query->lastQuery();
        return;
    }

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
    return {};
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

void SqlTableModel::rowChangedHandler(const QModelIndex &row)
{
    qDebug()<<"Called";
    m_query->finish();
    if(!m_query->exec())
        qWarning()<<"Couldn't refresh SqlTableModel. Query exec error. Error:"<<m_query->lastError().text()
                   <<". Query:"<<m_query->lastQuery();
    emit dataChanged(row,row);
}

int SqlTableModel::columnWidth(int c,int role, const QFont *font)
{
    if(!m_column_widths[c])
    {
        QFontMetrics default_metrics = QFontMetrics(QApplication::font());
        QFontMetrics fm = (font ? QFontMetrics(*font) : default_metrics);
        int ret = fm.horizontalAdvance(headerData(c,Qt::Horizontal,role).toString()
                                       + QLatin1String(" ^")) + 10;

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

void SqlTableModel::refresh()
{
    beginResetModel();
    QSqlQuery count_query(m_db.get_database());
    count_query.exec(QStringLiteral("SELECT COUNT(*) FROM processes_history"));
    if(count_query.next())
        m_row_count = count_query.value(0).toInt();
    else
        qWarning()<<"can't get row count of sql database. Error: "<<count_query.lastError().text();
    qDebug()<<"called";
    count_query.finish();
    m_query->finish();
    if(!m_query->exec())
        qWarning()<<"Couldn't refresh SqlTableModel. Query exec error. Error:"<<m_query->lastError().text()
                   <<". Query:"<<m_query->lastQuery();

    endResetModel();
    m_reset_timer.restart();

}

}
