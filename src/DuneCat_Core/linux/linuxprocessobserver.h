#ifndef LINUXPROCESSOBSERVER_H
#define LINUXPROCESSOBSERVER_H
#include <QObject>
#include <QThread>
#include <QAtomicInteger>
#include "dcprocessinfo.h"
class LinuxProcessObserver : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(LinuxProcessObserver)
public:
    explicit LinuxProcessObserver(QObject* parent = nullptr);
    ~LinuxProcessObserver();
    void run() override;
    QAtomicInteger<bool> need_exit{false};
private:
    int nl_connect();
    int set_proc_ev_listen(int nl_sock, bool enable);
    int handle_proc_ev(int nl_sock);
    int nl_sock;

signals:
    void process_created(const DCProcessInfo& process);
    void process_deleted(const DCProcessInfo& process);
};

#endif // LINUXPROCESSOBSERVER_H
