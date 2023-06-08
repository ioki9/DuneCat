#pragma once
#include <QObject>
#include <QThread>
#include <QAtomicInteger>
#include "processinfo.h"

namespace DuneCat
{
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
    void process_created(const ProcessInfo& process);
    void process_deleted(const ProcessInfo& process);
};
}
