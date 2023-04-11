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
    QString exe_path;
    quint32 pid;
    QDateTime creation_date;

};
#endif // DCPROCESSSTRUCTS_H
