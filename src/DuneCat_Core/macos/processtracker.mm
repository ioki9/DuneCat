#import "../processtracker.h"
#include <pwd.h>
#include <libproc.h>
#include <sys/kauth.h>
#import "OSXProcessObserver.h"
#import <Cocoa/Cocoa.h>

namespace DuneCat
{
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
            QDateTime timestamp;
            timestamp.setSecsSinceEpoch(proc.pbi_start_tvsec);
            proc_info.creation_date = timestamp;
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
{
    observer = [[OSXProcessObserver alloc] init];
    [observer setCallback:this];
    [observer enableNotification];
}

ProcessTracker::~ProcessTracker()
{
    [observer release];
    observer = nil;
}

std::vector<ProcessInfo> ProcessTracker::get_process_list()
{
    std::vector<ProcessInfo> result = get_gui_process_list();
    m_process_count = result.size();
    return result;
}

int ProcessTracker::get_process_count()
{
    if(m_process_count != -1)
        return m_process_count;
    pid_t pids[2048];
    std::vector<ProcessInfo> result{};
    int bytes = proc_listpids(PROC_ALL_PIDS,0,pids,sizeof(pids));
    m_process_count = bytes/sizeof(pids[0]);
    return m_process_count;
}

}