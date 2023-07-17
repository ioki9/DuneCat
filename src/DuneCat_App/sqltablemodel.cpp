
#include "sqltablemodel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QFontMetrics>
namespace DuneCat
{
SqlTableModel::SqlTableModel(const QSqlDatabase& db,QObject *parent)
    : QSqlQueryModel{parent},m_db{db}
{
}

void SqlTableModel::setQuery(const QString &query,const QSqlDatabase &db)
{
    QSqlQueryModel::setQuery(query, db);
    m_column_widths = std::vector<int>(QSqlQueryModel::columnCount(),0);
}

void SqlTableModel::setQuery(QSqlQuery &&query)
{
    QSqlQueryModel::setQuery(std::move(query));
    m_column_widths = std::vector<int>(QSqlQueryModel::columnCount(),0);
}

int SqlTableModel::rowCount(const QModelIndex &parent) const
{
//    QSqlQuery query{m_db};
//    query.exec(QStringLiteral("SELECT COUNT(*) FROM processes_history;"));

//    if(query.next())
//        return query.value(0).toInt();

//    return 0;
    return QSqlQueryModel::rowCount(parent);
}

QHash<int, QByteArray> SqlTableModel::generate_roles_from_fields()
{

    QHash<int, QByteArray> roles;
    for( int i = 0; i < record().count(); i ++) {
        roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
    }
    return roles;
}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
    if(role == DisplayDateRole)
    {
        int ts = QSqlQueryModel::data(index, Qt::DisplayRole).toInt();
        if(ts<0)
            ts *= -1;

        QDateTime date = QDateTime::fromSecsSinceEpoch(ts).toLocalTime();
        return QVariant{QDateTime::currentDateTime()};
    }
    else if(role == Qt::DisplayRole)
        return QSqlQueryModel::data(index, Qt::DisplayRole);
    return QVariant{};
}


QHash<int, QByteArray> SqlTableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[DisplayDateRole] = "displayDate";
    return roles;
}

int SqlTableModel::columnWidth(int c, const QFont *font)
{
    if(!m_column_widths[c])
    {
        QFontMetrics default_metrics = QFontMetrics(QApplication::font());
        QFontMetrics fm = (font ? QFontMetrics(*font) : default_metrics);
        int ret = fm.horizontalAdvance(headerData(c,Qt::Horizontal,Qt::DisplayRole).toString()
                                       + QLatin1String(" ^")) + 10;

        int rows = QSqlQueryModel::rowCount();
        for (int r{0};r<rows;++r)
            ret = qMax(ret,(fm.horizontalAdvance(data(QAbstractItemModel::createIndex(r,c), Qt::DisplayRole).toString())+6));

        m_column_widths[c] = ret;
    }
    return m_column_widths[c];
}

int SqlTableModel::columnWidth(int c, int pointSize)
{
    if(!m_column_widths[c])
    {
        auto* font = new QFont(QGuiApplication::font());
        font->setPointSize(pointSize);
        columnWidth(c,font);
    }
    return m_column_widths[c];
}

void SqlTableModel::refresh()
{
    QSqlQueryModel::setQuery(QSqlQueryModel::query().executedQuery(), m_db);
    while(canFetchMore())
        fetchMore();
}

}
