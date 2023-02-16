#pragma once
#include <QObject>
#include <memory>
#include "NetworkHeaders.pch"
#include "DCNetworkModels.h"

class DCStunClient : public QUdpSocket
{
    Q_OBJECT

public:
    explicit DCStunClient(QVector<DCEndPoint> stunServers, QObject *parent = nullptr);
    explicit DCStunClient(DCEndPoint stunServer, QObject *parent = nullptr);
    ~DCStunClient();
private:
    QVector<DCEndPoint> m_stunServers;
    QNetworkDatagram sendRequest(DCEndPoint stunAddr);
    DCStunHeader create_header(quint16_be message_type = quint16_be(0x0001));
    std::unique_ptr<DCEndPoint> m_mapped_address;
signals:


};
