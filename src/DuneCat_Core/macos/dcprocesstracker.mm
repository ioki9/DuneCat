#import "../dcprocesstracker.h"
#include <pwd.h>
#include <libproc.h>
#include <sys/kauth.h>
#import "OSXProcessObserver.h"
#import <Cocoa/Cocoa.h>

std::vector<DCProcessInfo> get_bsd_process_list()
{
    pid_t pids[2048];
    std::vector<DCProcessInfo> result{};
    int bytes = proc_listpids(PROC_ALL_PIDS,0,pids,sizeof(pids));
    int n_proc = bytes/sizeof(pids[0]);
    for(int i{0};i<n_proc;i++)
    {
        DCProcessInfo proc_info;
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

std::vector<DCProcessInfo> get_gui_process_list()
{
    std::vector<DCProcessInfo> result;
    for (NSRunningApplication *app in
           [[NSWorkspace sharedWorkspace] runningApplications])
      {
        DCProcessInfo proc;
        proc.name = QString::fromNSString([app localizedName]);
        proc.pid = [app processIdentifier];
        proc.file_path =  QString::fromNSString([[app executableURL] absoluteString]);
        result.push_back(proc);
      }
    return result;
}

OSXProcessObserver* observer;
DCProcessTracker::DCProcessTracker(QObject* parent)
{
    observer = [[OSXProcessObserver alloc] init];
    [observer setCallback:this];
    [observer enableNotification];
}

DCProcessTracker::~DCProcessTracker()
{
    [observer release];
    observer = nil;
}

std::vector<DCProcessInfo> DCProcessTracker::get_process_list()
{
    return get_gui_process_list();
}

int DCProcessTracker::get_process_count()
{
    return m_process_count;
}

QString DCProcessTracker::get_process_description(QString filepath)
{
    return "";
}

void DCProcessTracker::process_deleted_recieved(const DCProcessInfo &process)
{

}

void DCProcessTracker::process_created_recieved(const DCProcessInfo &process)
{

}

