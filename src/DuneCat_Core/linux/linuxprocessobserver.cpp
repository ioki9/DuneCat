#include "linuxprocessobserver.h"
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <signal.h>


int LinuxProcessObserver::nl_connect()
{
    int rc;
    int nl_sock;
    struct sockaddr_nl sa_nl;

    nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (nl_sock == -1) {
        perror("socket");
        return -1;
    }

    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = CN_IDX_PROC;
    sa_nl.nl_pid = getpid();

    rc = bind(nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl));
    if (rc == -1) {
        perror("bind");
        close(nl_sock);
        return -1;
    }

    return nl_sock;
}

/*
 * subscribe on proc events (process notifications)
 */
int LinuxProcessObserver::set_proc_ev_listen(int nl_sock, bool enable)
{
    int rc;
    struct __attribute__ ((aligned(NLMSG_ALIGNTO))) {
        struct nlmsghdr nl_hdr;
        struct __attribute__ ((__packed__)) {
            struct cn_msg cn_msg;
            enum proc_cn_mcast_op cn_mcast;
        };
    } nlcn_msg;

    memset(&nlcn_msg, 0, sizeof(nlcn_msg));
    nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
    nlcn_msg.nl_hdr.nlmsg_pid = getpid();
    nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

    nlcn_msg.cn_msg.id.idx = CN_IDX_PROC;
    nlcn_msg.cn_msg.id.val = CN_VAL_PROC;
    nlcn_msg.cn_msg.len = sizeof(enum proc_cn_mcast_op);

    nlcn_msg.cn_mcast = enable ? PROC_CN_MCAST_LISTEN : PROC_CN_MCAST_IGNORE;

    rc = send(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
    if (rc == -1) {
        perror("netlink send");
        return -1;
    }

    return 0;
}

/*
 * handle a single process event
 */
int LinuxProcessObserver::handle_proc_ev(int nl_sock)
{
    int rc;
    struct __attribute__ ((aligned(NLMSG_ALIGNTO))) {
        struct nlmsghdr nl_hdr;
        struct __attribute__ ((__packed__)) {
            struct cn_msg cn_msg;
            struct proc_event proc_ev;
        };
    } nlcn_msg;

    while (!need_exit.loadRelaxed()) {
        rc = recv(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
        if (rc == 0) {
            /* shutdown? */
            return 0;
        } else if (rc == -1) {
            if (errno == EINTR) continue;
            perror("netlink recv");
            return -1;
        }
        DCProcessInfo proc{};
        switch (nlcn_msg.proc_ev.what) {
        case proc_event::PROC_EVENT_NONE:
                qDebug()<<"set mcast listen ok\n";
                break;
        case proc_event::PROC_EVENT_FORK:
        {
            qDebug()<<"fork: parent tid="<< nlcn_msg.proc_ev.event_data.fork.parent_pid<<
                      " pid= "<<nlcn_msg.proc_ev.event_data.fork.parent_tgid<<
                      "-> child tid="<< nlcn_msg.proc_ev.event_data.fork.child_tgid<<
                      " pid= "<<nlcn_msg.proc_ev.event_data.fork.child_pid;

            proc.pid = nlcn_msg.proc_ev.event_data.fork.child_pid;
            emit process_created(proc);
            break;
        }

        case proc_event::PROC_EVENT_EXEC:
        {

            qDebug()<<"exec pid="<<nlcn_msg.proc_ev.event_data.exec.process_pid;
            proc.pid = nlcn_msg.proc_ev.event_data.exec.process_pid;
            emit process_created(proc);
            break;
        }

        case proc_event::PROC_EVENT_EXIT:
        {

            qDebug()<<"process exited:"<<nlcn_msg.proc_ev.event_data.exit.process_pid;
            proc.pid = nlcn_msg.proc_ev.event_data.exit.process_pid;
            emit process_deleted(proc);
            break;
        }
        default:
        {
            qDebug()<<"unhandled proc event\n";
            break;
        }
        }
    }
    return 0;
}

void LinuxProcessObserver::run()
{
    int rc = EXIT_SUCCESS;
    struct sigaction sa;
  //  signal(SIGINT, &on_sigint);

    sigaction(SA_RESTART,&sa,NULL);

    nl_sock = nl_connect();
    if (nl_sock == -1)
    {
        qDebug()<<"nl_sock returned -1 in LinuxProcessObserver constructor.";
        return;
    }

    rc = set_proc_ev_listen(nl_sock, true);
    if (rc == -1) {
        qDebug()<<"set_proc_ev_listen returned -1 in LinuxProcessObserver constructor.";
        return;
    }

    rc = handle_proc_ev(nl_sock);
    if (rc == -1) {
        qDebug()<<"handle_proc_ev returned -1 in LinuxProcessObserver constructor.";
        return;
    }

    set_proc_ev_listen(nl_sock, false);
}


LinuxProcessObserver::LinuxProcessObserver(QObject* parent) : QThread(parent)
{

}
LinuxProcessObserver::~LinuxProcessObserver()
{
    close(nl_sock);
}






