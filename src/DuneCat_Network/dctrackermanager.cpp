#include "dctrackermanager.h"

DCTrackerManager::DCTrackerManager(const DCEndPoint &tracker, QObject *parent)
    : QObject{parent}, m_tracker{tracker}
{
    DCEndPoint end{QHostAddress("77.72.169.212"),3478};
    socket = std::make_unique<DCStunClient>(end);
    connect(socket.get(),&DCStunClient::updated,this,&this->connectToTracker);
}

bool DCTrackerManager::connectToTracker()
{
    m_conn_req.transaction_id = QRandomGenerator::global()->generate();
    QByteArray data;
    QDataStream in(&data,QIODevice::ReadWrite);
    in.setByteOrder(QDataStream::BigEndian);
    in<<m_conn_req.protocol_id<<m_conn_req.action<<m_conn_req.transaction_id;
    qDebug()<<"bytes sent to tracker:"<<
              socket->writeDatagram(data,m_tracker.address,m_tracker.port);
    while(! socket->hasPendingDatagrams());
    data.clear();
    QNetworkDatagram datagram = socket->receiveDatagram();
    data = datagram.data();
    quint32 read_transaction_id;
    quint32 read_action;
    QDataStream out(&data,QIODevice::ReadOnly);
    out>>read_action>>read_transaction_id>>m_connection_id;
    if(m_conn_req.transaction_id == m_connection_id && data.size() >= 16 && read_action == 0)
        qDebug()<<"Connection response is correct.";
    DCTrackerAnnounce::Request announce;
    announce.action = 1;
    announce.connection_id = m_connection_id;
    announce.downloaded = announce.uploaded = announce.left = 0;
    for(quint8 i; i<20;i++)
    {
        announce.info_hash[i] = static_cast<quint8>(QRandomGenerator().global()->generate());
        announce.peer_id[i] = static_cast<quint8>(QRandomGenerator().global()->generate());
    }
    announce.transaction_id = QRandomGenerator().global()->generate();
    announce.event = 1;
    announce.ipv4 = 0;
    announce.num_want = quint32(-1);
    announce.port = socket->m_mapped_address->port;
    announce.key = 0;
    data.clear();
    QDataStream inAn(&data,QIODevice::WriteOnly);
    inAn<<announce.connection_id<<announce.action<<announce.transaction_id;
    for(quint8 i{0};i<20;i++)
        inAn<<announce.info_hash[i];
    for(quint8 i{0};i<20;i++)
        inAn<<announce.peer_id[i];

    inAn<<announce.downloaded<<announce.left<<announce.uploaded<<announce.event<<announce.ipv4<<announce.key
      <<announce.num_want<<announce.port;
    qDebug()<<"Announce bytes sent:"<<socket->writeDatagram(data,m_tracker.address,m_tracker.port);
    while(! socket->hasPendingDatagrams());
    qDebug()<<"Announce response recieved.";
    datagram = socket->receiveDatagram();
    data = datagram.data();
    QDataStream outAn(&data,QIODevice::ReadOnly);
    DCTrackerAnnounce::Response announResp;
    outAn>>announResp.action>>announResp.transaction_id>>announResp.interval
            >>announResp.leechers>>announResp.seeders;

    if(announResp.transaction_id == announce.transaction_id && announResp.action == 1 && data.size() >= 20)
        qDebug()<<"Announce response is correct.";
    QVector<quint32> ips;
    QVector<quint16> ports;
    for(quint8 n{0};(n*6)<(data.size()-20);n++)
    {
        ips.push_back(quint32());
        ports.push_back(quint16());
        outAn>>ips[n]>>ports[n];
    }
    connect(socket.get(),&QUdpSocket::readyRead,this,&DCTrackerManager::recieveMessage);
    for(quint8 i{0};i<ips.size();i++)
    {
        QHostAddress ip(ips[i]);
        if(ip != socket->m_mapped_address->address && port[i] != socket->m_mapped_address->port)
            socket->writeDatagram(QByteArray("Hello there"),ip,port);
    }

    return true;
}

void DCTrackerManager::recieveMessage()
{
    QNetworkDatagram datagram = socket->recieveDatagram();
    qDebug()<<"sender ip:port="<<datagram.senderAddress()<<":"<<datagram.senderPort();
    qDebug()<<"message:"<<datagram.data().data();
}
