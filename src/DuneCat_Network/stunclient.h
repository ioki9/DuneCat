#ifndef STUNCLIENT_H
#define STUNCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <memory>
#include "DCNetworkModels.h"

class STUNClient : public QObject
{
    Q_OBJECT

public:
    explicit STUNClient(QVector<DCEndPoint> stunServers, QObject *parent = nullptr);
    explicit STUNClient(DCEndPoint stunServer, QObject *parent = nullptr);
    ~STUNClient();
private:
    QVector<DCEndPoint> m_stunServers;
    DCSTUNHeader create_header(quint16 message_type = 0x0001);
    std::unique_ptr<DCEndPoint> m_mapped_address;
    std::unique_ptr<DCEndPoint> m_local_address;
signals:

};

#endif // STUNCLIENT_H
