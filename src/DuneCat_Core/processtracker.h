#pragma once
#include "essentialheaders.h"
#include "processinfo.h"
#include <shared_mutex>
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

    void apply_to_every_elem(const std::function<void(const ProcessInfo&)> callback) const
    {
        std::shared_lock lck{proc_vec_mutex};
        for(const auto& proc : m_processes)
            callback(proc);
    }

    template <typename... T>
    void apply_to_array(const std::function<void(const std::vector<ProcessInfo>& processes, T&&... args)> callback,T&&... args)
    {
        std::shared_lock lck{proc_vec_mutex};
        callback(m_processes,std::forward<T>(args)...);
    }

    void apply_to_array(const std::function<void(const std::vector<ProcessInfo>& processes)> callback)
    {
        std::shared_lock lck{proc_vec_mutex};
        callback(m_processes);
    }

private:
    explicit ProcessTracker(QObject *parent = nullptr);
    ~ProcessTracker() override;
    std::vector<ProcessInfo> gather_processes();

    std::atomic_uint64_t m_process_count{0};
    std::vector<ProcessInfo> m_processes{};
    mutable std::shared_mutex proc_vec_mutex;
signals:
    void process_created(const ProcessInfo& process);
    void process_deleted(const ProcessInfo& process);
private slots:
    void process_created_handler(const ProcessInfo& process);
    void process_deleted_handler(const ProcessInfo& process);
    //windows related functions
#ifdef Q_OS_WIN
    friend class WMIClient;
    static std::vector<ProcessInfo> get_winapi_process_list();
    static QString get_process_description(QStringView filepath);
#endif // Q_OS_WIN
};
}
