#pragma once
#include <QSortFilterProxyModel>
#include "processtablemodel.h"
#include <QObject>
#include <QFont>

namespace DuneCat
{
class SortFilterProcessModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProcessModel(QObject *parent = nullptr);
    Q_INVOKABLE void sort(int column,Qt::SortOrder order = Qt::AscendingOrder) override;
    Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnWidth(int c,const QFont *font = nullptr);
    Q_INVOKABLE int columnWidth(int c, int pointSize);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Q_INVOKABLE void refresh();

private:
    ProcessTableModel m_model;
};
}

