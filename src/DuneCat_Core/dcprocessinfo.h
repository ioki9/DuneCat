#ifndef DCPROCESSSTRUCTS_H
#define DCPROCESSSTRUCTS_H
#include <QtGlobal>
#include <QtCore>

#ifdef Q_OS_WIN
using DCProcessInfo = DCProcessInfoWin;
#endif
struct DCProcessInfoWin{
    QString name;
    QString decription;
    QString exe_path;
    QDateTime creation_date;
    quint32 pid;
};
#endif // DCPROCESSSTRUCTS_H
