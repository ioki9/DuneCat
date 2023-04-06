#ifndef DCPROCESSTRAKCER_H
#define DCPROCESSTRAKCER_H

#include "essentialheaders.h"
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
    int get_process_count() const {return m_active_processes.size();}
    void set_update_interval(int msec){m_update_interval = msec; m_update_timer.setInterval(msec);}
    QVector<ProcessInfo>& get_active_processes(){return m_active_processes;}
private:
    QTimer m_update_timer{};
    int m_update_interval{5000};
    QVector<ProcessInfo> m_active_processes{};
signals:
    void begin_process_list_update();
    void end_process_list_update();
private slots:
    void update_process_list();


};

#endif // DCPROCESSTRAKCER_H
