
#ifndef DCSORTFILTERPROXYPROCESSMODEL_H
#define DCSORTFILTERPROXYPROCESSMODEL_H

#include <QSortFilterProxyModel>
#include "dcprocesstablemodel.h"
#include <QObject>
#include <QFont>


class DCSortFilterProcessModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DCSortFilterProcessModel(QObject *parent = nullptr);
    Q_INVOKABLE virtual void sort(int column,Qt::SortOrder order = Qt::AscendingOrder) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnWidth(int c,const QFont *font = nullptr);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    DCProcessTableModel m_model;
};

#endif // DCSORTFILTERPROXYPROCESSMODEL_H
