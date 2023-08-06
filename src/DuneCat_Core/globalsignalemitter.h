#pragma once
#include <QObject>

namespace DuneCat
{
class GlobalSignalEmitter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(GlobalSignalEmitter)
public:
    static GlobalSignalEmitter *get_instance()
    {
        static GlobalSignalEmitter instance{};
        return &instance;
    }
signals:
    void db_wal_checkpoint();
private:
    explicit GlobalSignalEmitter(QObject *parent = nullptr);
};
}
