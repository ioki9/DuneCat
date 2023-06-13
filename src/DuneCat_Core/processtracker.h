#pragma once
#include "essentialheaders.h"
#include "processinfo.h"

namespace DuneCat{
class ProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ProcessTracker)
public:
    explicit ProcessTracker(QObject *parent = nullptr);
    ~ProcessTracker();
    std::vector<ProcessInfo> get_process_list();
    int get_process_count();
private:
    int m_process_count{-1};
signals:
    void process_created(const ProcessInfo& process);
    void process_deleted(const ProcessInfo& process);
private slots:
#if !defined(Q_OS_MAC)
    //slots for notifications on Linux and Windows
    void process_deleted_recieved(const ProcessInfo& process);
    void process_created_recieved(const ProcessInfo& process);
    friend class WMIClient;
    //windows related functions
#ifdef Q_OS_WINDOWS
    std::vector<ProcessInfo> get_winapi_process_list();
    QString get_process_description(QString filepath);
#endif // Q_OS_WINDOWS
#endif // !Q_OS_MAC
};
}
