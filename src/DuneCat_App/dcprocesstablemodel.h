#ifndef DCPROCESSTABLEMODEL_H
#define DCPROCESSTABLEMODEL_H

#include <QAbstractListModel>

class DCProcessTableModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DCProcessTableModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
};

#endif // DCPROCESSTABLEMODEL_H
