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
    std::vector<DCProcessInfo> get_active_processes();
    int get_process_count();
    QString get_process_description(QString filepath);
private:
    int m_process_count{-1};
signals:
    void process_created(const DCProcessInfo& process);
    void process_deleted(const DCProcessInfo& process);
private slots:
    //slots for wmi events on windows platform
    void process_deleted_recieved(const DCProcessInfo& process);
    void process_created_recieved(const DCProcessInfo& process);
};

#endif // DCPROCESSTRAKCER_H
