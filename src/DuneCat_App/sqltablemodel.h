#pragma once
#include <QSqlQueryModel>
#include <QSqlQuery>
#include "essentialheaders.h"

namespace DuneCat
{
class SqlTableModel : public QSqlQueryModel
{
    Q_OBJECT
    enum SqlRoles
    {
        DisplayDateRole = Qt::UserRole
    };

public:
    explicit SqlTableModel(const QSqlDatabase &db, QObject *parent = nullptr);
    void setQuery(const QString &query, const QSqlDatabase &db);
    void setQuery(QSqlQuery &&query);
    Q_INVOKABLE int columnWidth(int c,const QFont *font = nullptr);
    Q_INVOKABLE int columnWidth(int c, int pointSize);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
private:
    QHash<int, QByteArray> generate_roles_from_fields();
    std::vector<int> m_column_widths;
    QSqlDatabase m_db;
};


}
