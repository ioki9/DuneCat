#import "../processtracker.h"
#include <pwd.h>
#include <libproc.h>
#include <sys/kauth.h>
#import "OSXProcessObserver.h"
#import <Cocoa/Cocoa.h>

namespace DuneCat
{

bool procinfo_less_pid_comp(const ProcessInfo& lhs,const ProcessInfo& rhs)
{
    return lhs.pid < rhs.pid;
}

std::vector<ProcessInfo> get_bsd_process_list()
{
    pid_t pids[2048];
    std::vector<ProcessInfo> result{};
    int bytes = proc_listpids(PROC_ALL_PIDS,0,pids,sizeof(pids));
    int n_proc = bytes/sizeof(pids[0]);
    for(int i{0};i<n_proc;i++)
    {
        ProcessInfo proc_info;
        struct proc_bsdinfo proc;
        int st = proc_pidinfo(pids[i],PROC_PIDTBSDINFO,0,&proc,PROC_PIDTBSDINFO_SIZE);
        if(st == PROC_PIDTBSDINFO_SIZE)
        {
            proc_info.creation_time = QDateTime::fromSecsSinceEpoch(proc.pbi_start_tvsec);
            proc_info.pid = pids[i];
            struct passwd* pwd = getpwuid(proc.pbi_uid);
            proc_info.owner_user = pwd->pw_name;
            proc_info.name = proc.pbi_comm;
            proc_info.description = proc.pbi_name;
            result.push_back(proc_info);
        }
    }
    return result;
}

std::vector<ProcessInfo> get_gui_process_list()
{
    std::vector<ProcessInfo> result;
    for (NSRunningApplication *app in
           [[NSWorkspace sharedWorkspace] runningApplications])
      {
        ProcessInfo proc;
        proc.name = QString::fromNSString([app localizedName]);
        proc.pid = [app processIdentifier];
        proc.file_path =  QString::fromNSString([[app executableURL] absoluteString]);
        result.push_back(proc);
      }
    return result;
}

OSXProcessObserver* observer;
ProcessTracker::ProcessTracker(QObject* parent)
: QObject{parent}
{
    m_processes = gather_processes();
    if(m_processes.empty())
    {
        qFatal()<<"Couldn't gather process list. Exiting...";
        QCoreApplication::exit(-1);
    }
    std::sort(m_processes.begin(),m_processes.end(),procinfo_less_pid_comp);
    observer = [[OSXProcessObserver alloc] init];
    [observer setCallback:this];
    [observer enableNotification];

    connect(this,&ProcessTracker::process_created,this,&ProcessTracker::process_created_handler);
    connect(this,&ProcessTracker::process_deleted,this,&ProcessTracker::process_deleted_handler);
}

ProcessTracker::~ProcessTracker()
{
    [observer release];
    observer = nil;
}

void ProcessTracker::get_process_list(std::vector<ProcessInfo>& list_out) const
{
    std::shared_lock lck{proc_vec_mutex};
    list_out = m_processes;
}

std::vector<ProcessInfo> ProcessTracker::gather_processes()
{
    if(Q_LIKELY(m_process_count != 0))
        return m_processes;
    std::vector<ProcessInfo> result = get_gui_process_list();
    m_process_count = result.size();
    return result;
}

int ProcessTracker::get_process_count()
{
    if(Q_LIKELY(m_process_count != 0))
        return m_process_count;
    pid_t pids[2048];
    std::vector<ProcessInfo> result{};
    int bytes = proc_listpids(PROC_ALL_PIDS,0,pids,sizeof(pids));
    m_process_count = bytes/sizeof(pids[0]);
    return m_process_count;
}

void ProcessTracker::process_created_handler(const ProcessInfo &process)
{
    std::unique_lock lck{proc_vec_mutex};
    m_process_count += 1;
    auto it = std::upper_bound(m_processes.begin(), m_processes.end(), process,
                               [](auto& value, auto& elem ) { return value.pid < elem.pid;});
    m_processes.insert(it, process);
}

void ProcessTracker::process_deleted_handler(const ProcessInfo &process)
{
    std::unique_lock lck{proc_vec_mutex};
    auto it = std::lower_bound(m_processes.begin(),m_processes.end(),process,
                               [](auto& value, auto& elem ) { return value.pid < elem.pid;});
    if(it == m_processes.end())
    {
        qDebug()<<"process not found. PID: "<<process.pid;
        return;

    }
    m_process_count -= 1;
    m_processes.erase(it);
}

}
