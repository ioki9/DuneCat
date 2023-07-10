#pragma once
#include <QtGlobal>
#include <QtCore>
#include <QPixmap>
namespace DuneCat{
struct ProcessInfo{
    QString name;
    QString description;
    QString file_path;
    QString command_line;
    QString owner_user;
    QString owner_domain;
    quint32 pid;
    QDateTime termination_time;
    QDateTime creation_time;
    QPixmap icon;
};

}
