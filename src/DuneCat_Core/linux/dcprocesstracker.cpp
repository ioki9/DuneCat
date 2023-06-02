#include "dcprocesstracker.h"
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/time.h>
//for reference: https://man7.org/linux/man-pages/man5/proc.5.html
//           or  https://kb.novaordis.com/index.php//proc/pid/stat#Field_2_-_Executable_File_Name
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

QDateTime get_start_time(const QFileInfo& proc_dir)
{
    QDateTime result{};
    struct timeval tv;
    unsigned long long uptime{};
    QFile uptime_file("/proc/uptime");
    static time_t boot_time;
    if (!uptime_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return result;
    uptime = uptime_file.readLine().toULongLong();
    gettimeofday(&tv, 0);
    boot_time = tv.tv_sec - uptime;
    unsigned long long start_stat = get_stat_params(proc_dir,{SP_starttime})[0].toULongLong() / sysconf(_SC_CLK_TCK);
    result.setSecsSinceEpoch(boot_time - start_stat);
    return result;
}

DCProcessTracker::DCProcessTracker(QObject *parent)
{

}

DCProcessTracker::~DCProcessTracker()
{

}

std::vector<DCProcessInfo> DCProcessTracker::get_process_list()
{
    std::vector<DCProcessInfo> proc_list{};
    const QDir proc_dir(QString("/proc"));
    if(!proc_dir.exists())
        return proc_list;
    static QRegularExpression pat("^\\d+$");
    const QFileInfoList proc_ls{proc_dir.entryInfoList()};
    for(const auto& info : proc_ls)
    {
        if(!pat.match(info.baseName()).hasMatch())
          continue;
        //get info from /proc/[pid]/stat
        std::vector<StatParams> params{SP_pid,SP_comm,SP_starttime};
        std::vector<QString> param_res = get_stat_params(info,params);
        //can't get the most relevant information so continue.
        if(params.empty())
            continue;

        DCProcessInfo proc;
        proc.pid = param_res[0].toInt();
        proc.name = param_res[1];
        //getting user
        struct stat sb;
        ssize_t buff {PATH_MAX};
        if(lstat(QByteArray(info.absoluteFilePath().toUtf8()),&sb) != -1)
             proc.owner_user = get_user(sb.st_uid);

        //get path
        if (sb.st_size != 0)
            buff = sb.st_size + 1;
        char path[buff];
        QByteArray path_to_proc = info.absoluteFilePath().toLatin1() + "/exe";
        ssize_t nbytes = readlink(path_to_proc.data(),path,buff);
        if(nbytes != -1)
            proc.file_path = QByteArray(path,nbytes);
        proc.creation_date = get_start_time(info);

        proc_list.push_back(proc);
    }
    return proc_list;
}



int DCProcessTracker::get_process_count()
{
    return m_process_count;
}


