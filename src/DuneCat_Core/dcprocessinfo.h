#ifndef DCPROCESSSTRUCTS_H
#define DCPROCESSSTRUCTS_H
#include <QtGlobal>
#include <QtCore>
struct DCProcessInfo{
    QString name;
    QString description;
    QString file_path;
    QString command_line;
    QString owner_user;
    QString owner_domain;
    quint32 pid;
    QDateTime termination_date;
    QDateTime creation_date;

};
#endif // DCPROCESSSTRUCTS_H
