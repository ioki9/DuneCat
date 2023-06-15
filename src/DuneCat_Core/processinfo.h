#pragma once
#include <QtGlobal>
#include <QtCore>
#include <QPixmap>
namespace DuneCat{
struct ProcessInfo{
    QString name;
    [[maybe_unused]] QString description;
    QString file_path;
    [[maybe_unused]] QString command_line;
    [[maybe_unused]] QString owner_user;
    [[maybe_unused]] QString owner_domain;
    quint32 pid;
    [[maybe_unused]] QDateTime termination_date;
    QDateTime creation_date;
    [[maybe_unused]] QPixmap icon;
};
}
