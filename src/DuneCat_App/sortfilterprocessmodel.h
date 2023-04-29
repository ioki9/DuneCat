#ifndef SORTFILTERPROCESSMODEL_H
#define SORTFILTERPROCESSMODEL_H

#include <QSortFilterProxyModel>
#include <QObject>
#include "dcprocesstablemodel.h"

class SortFilterProcessModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProcessModel(QObject *parent = nullptr);
};

#endif // SORTFILTERPROCESSMODEL_H
