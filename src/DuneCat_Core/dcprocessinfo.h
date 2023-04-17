#ifndef DCPROCESSSTRUCTS_H
#define DCPROCESSSTRUCTS_H
#include <QtGlobal>
#include <QtCore>

#ifdef Q_OS_WIN
struct DCProcessInfoWin;
using DCProcessInfo = DCProcessInfoWin;
#endif
struct DCProcessInfoWin{
    QString name;
    QString decription;
    QString file_path;
    QString command_line;
    QString owner_user;
    QString owner_domain;
    quint32 pid;
    QDateTime termination_date;
    QDateTime creation_date;

};
#endif // DCPROCESSSTRUCTS_H
