#ifndef DCPROCESSTABLEMODEL_H
#define DCPROCESSTABLEMODEL_H
#include <QAbstractListModel>
#include "dcprocesstracker.h"
#include <vector>

class DCProcessTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum class EColumns
    {
        process_name,
        description,
        pid,
        user_name,
        domain_name
    };
    explicit DCProcessTableModel(QObject *parent = nullptr);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Q_INVOKABLE int columnWidth(int c,const QFont *font = nullptr);
    Q_INVOKABLE int columnWidth(int c, int pointSize);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QHash<int,QByteArray> roleNames() const override;
private:
    QMutex mutex;
    std::vector<int> m_column_widths;
    std::vector<DCProcessInfo> m_processes;
    DCProcessTracker *m_proc_tracker = nullptr;
    int m_column_count{5};
private slots:
    void add_new_process(const DCProcessInfo& proc);
    void remove_process(const DCProcessInfo& proc);
};

#endif // DCPROCESSTABLEMODEL_H
