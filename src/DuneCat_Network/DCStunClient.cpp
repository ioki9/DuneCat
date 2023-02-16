#include "DCStunClient.h"

DCStunClient::DCStunClient(QVector<DCEndPoint> stunServers, QObject *parent)
    : QObject{parent}, m_stunServers{stunServers}
{

}
DCStunClient::DCStunClient(DCEndPoint stunServer, QObject *parent)
    : QObject{parent}, m_stunServers{stunServer}
{

}
DCStunClient::~DCStunClient()
{
}

QNetworkDatagram DCStunClient::sendRequest(DCEndPoint stunAddr)
{
    bind(stunAddr.address,stunAddr.port);
    //writeDatagram()
}

DCStunHeader DCStunClient::create_header(quint16_be message_type)
{
    QRandomGenerator generator;
    DCStunHeader header;
    //transaction id should be random
    for(quint8 i{0}; i<3; i++)
        header.transaction_id[i] = generator.generate();

    header.msg_type = message_type;
    return header;
}
