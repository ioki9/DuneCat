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
    m_connect.transaction_id = QRandomGenerator::global()->generate();
    QByteArray data;
    qDebug()<<m_connect.protocol_id<<"action"<<m_connect.action;
    QDataStream in(&data,QIODevice::ReadWrite);
    in.setByteOrder(QDataStream::BigEndian);
    in<<m_connect.protocol_id<<m_connect.action<<m_connect.transaction_id;
    qDebug()<<"bytes sent to tracker:"<<
              socket->writeDatagram(data,m_tracker.address,m_tracker.port);
    while(! socket->hasPendingDatagrams());
    data.clear();
    QNetworkDatagram datagram = socket->receiveDatagram();
    data = datagram.data();
    quint32 read_transaction_id;
    quint64 read_connection_id;
    quint32 read_action;
    QDataStream out(&data,QIODevice::ReadOnly);
    out>>read_action>>read_transaction_id>>read_connection_id;
    if(m_connect.transaction_id == read_transaction_id)
        qDebug()<<"Success";
    DCTrackerAnnounce::Request announce;
    announce.action = 1;
    announce.connection_id = read_connection_id;
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

    return true;
}

