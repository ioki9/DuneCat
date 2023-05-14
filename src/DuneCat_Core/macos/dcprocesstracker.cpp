#include "dcprocesstracker.h"
#include <libproc.h>
#include <pwd.h>
DCProcessTracker::DCProcessTracker(QObject* parent)
{

}

DCProcessTracker::~DCProcessTracker()
{

}

std::vector<DCProcessInfo> DCProcessTracker::get_active_processes()
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
