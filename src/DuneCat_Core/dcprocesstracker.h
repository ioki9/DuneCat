#ifndef DCPROCESSTRAKCER_H
#define DCPROCESSTRAKCER_H

#include "essentialheaders.h"
#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#elif defined(Q_OS_MAC)
#include <Cocoa/Cocoa.h>
#endif

struct ProcessInfo
{
    QString name;
    quint32 process_id;
    quint32 thread_count;
    quint32 parent_process_id;
    quint32 prority_class;
    qint32 priority_base;

};

class DCProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DCProcessTracker)
public:
    explicit DCProcessTracker(QObject *parent = nullptr);
    int get_process_count() const {return m_active_processes.size();}
    void get_process_list();
    QVector<ProcessInfo>& get_active_processes(){return m_active_processes;}
private:

    QVector<ProcessInfo> m_active_processes;
signals:

};

#endif // DCPROCESSTRAKCER_H
