#include "stunclient.h"
#include <QRandomGenerator>

STUNClient::STUNClient(QVector<DCEndPoint> stunServers, QObject *parent)
    : QObject{parent}, m_stunServers{stunServers}
{

}
STUNClient::STUNClient(DCEndPoint stunServer, QObject *parent)
    : QObject{parent}, m_stunServers{stunServer}
{
}
STUNClient::~STUNClient()
{
}



DCSTUNSHeader STUNClient::create_header(quint16 message_type)
{
    DCSTUNHeader header;
    //transaction id should be random
    for(quint8 i{0}; i<3; i++)
        header.transaction_id[i] = QRandomGenerator.generate();

    header.msg_type = htons(message_type);
    return header;
}
