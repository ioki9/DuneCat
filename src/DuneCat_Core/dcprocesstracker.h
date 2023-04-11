#ifndef DCPROCESSTRAKCER_H
#define DCPROCESSTRAKCER_H

#include "essentialheaders.h"
#include "dcprocessinfo.h"
#ifdef Q_OS_WINDOWS
#define _WIN32_DCOM
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#elif defined(Q_OS_MAC)//endif Q_OS_WINDOWS
#include <Cocoa/Cocoa.h>
#endif

struct ProcessInfo
{
    QString name;
    quint32 process_id;
    quint32 thread_count;
    quint32 prority_class;
    qint32 priority_base;

};

class DCProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DCProcessTracker)
public:
    explicit DCProcessTracker(QObject *parent = nullptr);
    QVector<DCProcessInfo> get_active_processes();
    int get_process_count();
private:
    int m_process_count{-1};
signals:
    void new_process_created(const DCProcessInfo& process);
private slots:
    void process_created_recieved(const DCProcessInfo& process);
};

#endif // DCPROCESSTRAKCER_H
