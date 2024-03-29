#include "processtracker.h"
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "linuxprocessobserver.h"
#include <QStringTokenizer>
namespace DuneCat
{
bool procinfo_less_pid_comp(const ProcessInfo& lhs,const ProcessInfo& rhs)
{
    return lhs.pid < rhs.pid;
}

//see: https://man7.org/linux/man-pages/man5/proc.5.html
// or  https://kb.novaordis.com/index.php//proc/pid/stat#Field_2_-_Executable_File_Name
enum StatParams : uint8_t
{
    SP_pid = 0,
    SP_comm,
    SP_state,
    SP_ppid,
    SP_pgrp,
    SP_session,
    SP_tty_nr,
    SP_tpgid,
    SP_flags,
    SP_minflt,
    SP_cminflt,
    SP_majflt,
    SP_cmajflt,
    SP_utime,
    SP_stime,
    SP_cutime,
    SP_cstime,
    SP_priority,
    SP_nice,
    SP_num_threads,
    SP_itrealvalue,
    SP_starttime,
    SP_vsize,
    SP_rss,
    SP_rsslim,
    SP_startcode,
    SP_endcode,
    SP_startstack,
    SP_kstkesp,
    SP_kstkeip,
    SP_signal,
    SP_blocked,
    SP_sigignore,
    SP_sigcatch,
    SP_wchan,
    SP_nswap,
    SP_cnswap,
    SP_exit_signal,
    SP_processor,
    SP_rt_priority,
    SP_policy,
    SP_delayacct_blkio_ticks,
    SP_guest_time,
    SP_cguest_time,
    SP_start_data,
    SP_end_data,
    SP_start_brk,
    SP_arg_start,
    SP_arg_end,
    SP_env_start,
    SP_env_end,
    SP_exit_code,
    SP_MAX_STATPARAMS
};

std::vector<QString> get_stat_params(const QFileInfo& dir_info,const std::vector<StatParams>& param_idxs)
{
    std::vector<QString> result(param_idxs.size(),"");

    QFile stat(dir_info.absoluteFilePath() + "/stat");
    if (!stat.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::vector<QString>{};

    QByteArray pid{dir_info.baseName().toUtf8()};
    QByteArray name{""};
    QByteArray line = stat.readLine();
    if(line.isEmpty())
        return std::vector<QString>{};
    size_t name_pid_offset{0};

    //it's easier to get first two parameters of /proc/[pid]/stat elsewhere and parse the rest of it with ' ' delimeter
    QFile comm(dir_info.absoluteFilePath()  + "/comm");
    if(comm.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        name = comm.readLine();
        name_pid_offset = name.size() + pid.size() + 3;
    }
    else //in case we can't open /proc/[pid]/comm for whatever reason, we parse /proc/[pid]/stat for our needs
    {
        size_t k{0};
        for(;k<line.size();k++)
        {
            if(line[k]==' ')
            {
                k++;
                int l_count{1};
                int r_count{0};
                while(l_count != r_count && k != line.size())
                {
                    k++;
                    if(line[k] == ')')
                        r_count++;
                    else if(line[k] == '(')
                        l_count++;
                    name+=line[k];
                }
                name.chop(1);
                break;
            }
        }
        name_pid_offset = k+2;
    }

    QList<QByteArray> params = line.sliced(name_pid_offset).split(' ');
    params.push_front(name);
    params.push_front(pid);

    for(size_t i{0};i<param_idxs.size();i++)
        result[i] = params[param_idxs[i]];

    comm.close();
    stat.close();
    return result;
}

QString get_user(uid_t uid)
{
    QString user{""};
    struct passwd *pwd;
    if ((pwd = getpwuid(uid)) != NULL)
        user = pwd->pw_name;
    return user;
}

QDateTime get_proc_start_time(const QFileInfo& proc_dir)
{
    QDateTime result;

    static time_t boot_time{0};
    if(Q_UNLIKELY(!boot_time))
    {
        QFile uptime_file("/proc/uptime");
        if (!uptime_file.open(QIODevice::ReadOnly | QIODevice::Text))
            return result;
        double uptime = QString(uptime_file.readLine()).split(' ')[0].toDouble();
        boot_time = static_cast<double>(QDateTime::currentMSecsSinceEpoch())/1000 - uptime;
    }

    if(!proc_dir.exists())
        return QDateTime{};
    std::vector<QString> stats = get_stat_params(proc_dir,{SP_starttime});
    if(stats.empty())
        return QDateTime{};
    unsigned long long start_stat = stats[0].toULongLong() / sysconf(_SC_CLK_TCK);
    result.setSecsSinceEpoch(boot_time + start_stat);
    result.setTimeZone(QTimeZone::systemTimeZone());
    return result;
}

bool get_info_by_pid(quint32 pid, ProcessInfo& out)
{
    QFileInfo dir(QString("/proc/") + QString::number(pid));
    if(!dir.exists())
        return false;

    //get info from /proc/[pid]/stat
    std::vector<StatParams> params{SP_comm};
    std::vector<QString> param_res = get_stat_params(dir,params);
    //can't get the most relevant information so continue.
    if(params.empty())
    {
        qDebug()<<"params empty";
        return false;
    }
    out.name = param_res[0];
    //getting user
    struct stat sb;
    ssize_t buff {PATH_MAX};
    if(lstat(QByteArray(dir.absoluteFilePath().toUtf8()),&sb) != -1)
        out.owner_user = get_user(sb.st_uid);

    //get path
    if (sb.st_size != 0)
        buff = sb.st_size + 1;
    char path[buff];
    QByteArray path_to_proc = dir.absoluteFilePath().toLatin1() + "/exe";
    ssize_t nbytes = readlink(path_to_proc.data(),path,buff);
    if(nbytes != -1)
        out.file_path = QByteArray(path,nbytes);
    out.creation_time = get_proc_start_time(dir);
    return true;
}

LinuxProcessObserver* observer;
ProcessTracker::ProcessTracker(QObject *parent) : QObject(parent)
{
    m_processes = gather_processes();
    if(m_processes.empty())
    {
        qFatal()<<"Couldn't gather process list. Exiting...";
        QCoreApplication::exit(-1);
    }
    std::sort(m_processes.begin(),m_processes.end(),procinfo_less_pid_comp);

    observer = new LinuxProcessObserver(this);
    connect(observer, &DuneCat::LinuxProcessObserver::finished, observer, &QObject::deleteLater);
    connect(observer,&DuneCat::LinuxProcessObserver::process_created,this,&DuneCat::ProcessTracker::process_created_handler);
    connect(observer,&DuneCat::LinuxProcessObserver::process_deleted,this,&DuneCat::ProcessTracker::process_deleted_handler);

    observer->start();
}

ProcessTracker::~ProcessTracker()
{
    observer->need_exit.storeRelaxed(true);
    observer->quit();
    observer->wait();
}

std::vector<ProcessInfo> ProcessTracker::gather_processes()
{
    if(Q_LIKELY(m_process_count != 0))
        return m_processes;
    std::vector<ProcessInfo> proc_list{};
    const QDir proc_dir(QString("/proc"));
    if(!proc_dir.exists())
        return proc_list;
    static QRegularExpression pat("^\\d+$");
    const QFileInfoList proc_ls{proc_dir.entryInfoList()};
    for(const auto& info : proc_ls)
    {
        if(!pat.match(info.baseName()).hasMatch())
            continue;

        ProcessInfo proc;
        proc.pid = info.baseName().toInt();
        if(!get_info_by_pid(proc.pid,proc))
            continue;
        proc_list.push_back(proc);
    }
    return proc_list;
}

void ProcessTracker::get_process_list(std::vector<ProcessInfo>& list_out) const
{
    std::shared_lock lck{proc_vec_mutex};
    list_out = m_processes;
}

int ProcessTracker::get_process_count()
{
    if(Q_LIKELY(m_process_count != 0))
        return m_process_count;
    const QDir proc_dir(QString("/proc"));
    if(!proc_dir.exists())
        return -1;
    static QRegularExpression pat("^\\d+$");
    const QFileInfoList proc_ls{proc_dir.entryInfoList()};
    m_process_count = 0;
    for(const auto& info : proc_ls)
    {
        if(!pat.match(info.baseName()).hasMatch())
            continue;
        m_process_count++;
    }
    return m_process_count;
}


void ProcessTracker::process_created_handler(const ProcessInfo &process)
{
    std::unique_lock lck{proc_vec_mutex};
    m_process_count +=1;
    auto it = std::upper_bound(m_processes.begin(), m_processes.end(), process,
                               [](auto& value, auto& elem ) { return value.pid < elem.pid;});
    ProcessInfo proc = process;
    if(!get_info_by_pid(process.pid,proc))
        return;
    m_processes.insert(it, proc);
    emit process_created(proc);
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

    ProcessInfo proc = process;
    proc.creation_time = it->creation_time;
    m_process_count -=1;
    m_processes.erase(it);
    emit process_deleted(proc);
}

}
