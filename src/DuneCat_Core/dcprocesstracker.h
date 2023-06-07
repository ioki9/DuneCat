#ifndef DCPROCESSTRAKCER_H
#define DCPROCESSTRAKCER_H
#include "essentialheaders.h"
#include "dcprocessinfo.h"
class DCProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DCProcessTracker)
public:
    explicit DCProcessTracker(QObject *parent = nullptr);
    ~DCProcessTracker();
    std::vector<DCProcessInfo> get_process_list();
    int get_process_count();
private:
    QThread workerThread;
    int m_process_count{-1};
signals:
    void process_created(const DCProcessInfo& process);
    void process_deleted(const DCProcessInfo& process);
private slots:
#if !defined(Q_OS_MAC)
    //slots for wmi events on windows platform
    void process_deleted_recieved(const DCProcessInfo& process);
    void process_created_recieved(const DCProcessInfo& process);
    //get description for windows processes
    QString get_process_description(QString filepath);
#endif
};

#endif // DCPROCESSTRAKCER_H
