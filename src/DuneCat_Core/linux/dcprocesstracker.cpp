#include "dcprocesstracker.h"
#include <unistd.h>
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
std::vector<QString> get_proc_stats(const QString& dir_path,const std::vector<StatParams>& param_idxs)
{
    std::vector<QString> result(param_idxs.size(),"");
    QFile stat(dir_path + "/stat");
    if (!stat.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::vector<QString>{};
    QByteArray line = stat.readLine();
    QByteArray pid_info{""};
    QByteArray name_info{""};
    size_t k{0};
    for(;k<line.size();k++)
    {
        if(line[k]==' ')
        {
            k++;
            while(line[k] != ')')
            {
                k++;
                name_info+=line[k];
            }
            name_info.chop(1);
            break;
        }
        pid_info+=line[k];
    }
    QList<QByteArray> params = line.sliced(k+2).split(' ');
    params.push_front(name_info);
    params.push_front(pid_info);
    for(size_t i{0};i<param_idxs.size();i++)
        result[i] = params[param_idxs[i]];
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
        std::vector<StatParams> params{SP_pid,SP_comm,SP_starttime};
        std::vector<QString> param_res = get_proc_stats(info.absoluteFilePath(),params);
        if(params.empty())
            continue;

        DCProcessInfo proc;
        proc.pid = param_res[0].toInt();
        proc.name = param_res[1];
        //get path
        ssize_t buff = 256;
        char path[buff];
        QByteArray path_to_proc = info.absoluteFilePath().toLatin1() + "/exe";
        ssize_t nbytes = readlink(path_to_proc.data(),path,buff);
        if(nbytes != -1)
            proc.file_path = QByteArray(path,nbytes);

        proc_list.push_back(proc);

    }
    return proc_list;
}



int DCProcessTracker::get_process_count()
{
    return m_process_count;
}


