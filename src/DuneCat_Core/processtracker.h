#pragma once
#include "essentialheaders.h"
#include "processinfo.h"

namespace DuneCat{
class ProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ProcessTracker)
public:
    static ProcessTracker *get_instance()
    {
        static ProcessTracker instance{};
        return &instance;
    }
    int get_process_count();
    void get_process_list(std::vector<ProcessInfo>& list_out) const;

private:
    explicit ProcessTracker(QObject *parent = nullptr);
    ~ProcessTracker() override;
    std::vector<ProcessInfo> gather_processes();

    std::atomic_uint64_t m_process_count{0};
    std::vector<ProcessInfo> m_processes{};
    mutable std::mutex proc_vec_mutex;
signals:
    void process_created(const ProcessInfo& process);
    void process_deleted(const ProcessInfo& process);
private slots:
    void process_created_handler(const ProcessInfo& process);
    void process_deleted_handler(const ProcessInfo& process);
#if !defined(Q_OS_MAC)
    //slots for notifications on Linux and Windows
    void process_deleted_recieved(const ProcessInfo& process);
    void process_created_recieved(const ProcessInfo& process);
    //windows related functions
    #ifdef Q_OS_WIN
        friend class WMIClient;
        static std::vector<ProcessInfo> get_winapi_process_list();
        static QString get_process_description(QStringView filepath);
    #endif // Q_OS_WIN
#endif // !Q_OS_MAC
};
}
