#ifndef DCPROCESSTRAKCER_H
#define DCPROCESSTRAKCER_H

#include "essentialheaders.h"

class DCProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DCProcessTracker)
public:
    explicit DCProcessTracker(QObject *parent = nullptr);
    void get_process_list();
private:
    struct ProcessInfoWin
    {
        char name[256];
        quint32 process_id;
        quint32 thread_count;
        quint32 parent_process_id;
        quint32 prority_class;
        qint32 priority_base;
    };

    QVector<ProcessInfoWin> m_active_processes{};
signals:

};

#endif // DCPROCESSTRAKCER_H
