#ifndef DCPROCESSTABLEMODEL_H
#define DCPROCESSTABLEMODEL_H
#include <QAbstractListModel>
#include "dcprocesstracker.h"
#include <QMutex>
#include <QMutexLocker>
class DCProcessTableModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ~DCProcessTableModel(){m_proc_tracker->deleteLater();}
    explicit DCProcessTableModel(QObject *parent = nullptr);
    enum{
        ProcessNameRole = Qt::UserRole,
        PIDRole
    };
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual QHash<int,QByteArray> roleNames() const override;
private:
    QVector<DCProcessInfo> m_processes;
    DCProcessTracker *m_proc_tracker = nullptr;
private slots:
    void add_new_process(const DCProcessInfo& proc);
};

#endif // DCPROCESSTABLEMODEL_H
