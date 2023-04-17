#ifndef DCPROCESSTABLEMODEL_H
#define DCPROCESSTABLEMODEL_H
#include <QAbstractListModel>
#include "dcprocesstracker.h"
#include <vector>

class DCProcessTableModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DCProcessTableModel(QObject *parent = nullptr);
    enum{
        ProcessNameRole = Qt::UserRole,
        PIDRole,
        TerminationDateRole,
        CreationDateRole,
        FilePathRole,
        CommandLineRole,
        OwnerUserRole,
        OwnerDomainRole
    };
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int,QByteArray> roleNames() const override;
private:
    QMutex mutex;
    std::vector<DCProcessInfo> m_processes;
    DCProcessTracker *m_proc_tracker = nullptr;
private slots:
    void add_new_process(const DCProcessInfo& proc);
    void remove_process(const DCProcessInfo& proc);
};

#endif // DCPROCESSTABLEMODEL_H
