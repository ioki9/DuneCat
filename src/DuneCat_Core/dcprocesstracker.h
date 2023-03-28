#ifndef DCPROCESSTRAKCER_H
#define DCPROCESSTRAKCER_H

#include <QObject>
#include <QProcess>

class DCProcessTracker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DCProcessTracker)
public:
    explicit DCProcessTracker(QObject *parent = nullptr);
    void get_process_list();
signals:

};

#endif // DCPROCESSTRAKCER_H
