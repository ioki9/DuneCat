#include "dctrackermanager.h"
#include "DCTools.h"

DCTrackerManager::DCTrackerManager(const DCEndPoint &tracker, QObject *parent)
    : QObject{parent}, m_current_tracker{tracker}
{
    m_trackerTimer = new QTimer();
    DCEndPoint end{QHostAddress("77.72.169.212"),3478};
    m_socket = std::make_unique<DCStunClient>(end);
    connect(m_socket.get(),&DCStunClient::updated,this,&DCTrackerManager::initParameters);
    connect(m_socket.get(),&DCStunClient::updated,this,&DCTrackerManager::connectionRequest);
    connect(this,&DCTrackerManager::connSucceed,this,&DCTrackerManager::sendAnnounce);
    connect(this, &DCTrackerManager::endpointsUpdate,this, &DCTrackerManager::sendMessage);
}

void DCTrackerManager::initParameters()
{
    m_connect.request.transaction_id = QRandomGenerator::global()->generate();
    m_announce.request.action = 1;
    m_announce.request.downloaded = m_announce.request.uploaded = m_announce.request.left = 0;
    for(quint8 i; i<20;i++)
    {
        m_announce.request.info_hash[i] = static_cast<quint8>(i*346253);
        m_announce.request.peer_id[i] = static_cast<quint8>(QRandomGenerator().global()->generate());
    }
    m_announce.request.transaction_id = QRandomGenerator().global()->generate();
    m_announce.request.event = 1;
    m_announce.request.ipv4 = 0;
    m_announce.request.num_want = quint32(-1);
    m_announce.request.port = m_socket->m_mapped_address->port;
    m_announce.request.key = 0;
}

bool DCTrackerManager::setupConnection()
{

}

void DCTrackerManager::handleDatagram()
{
    QNetworkDatagram datagram = m_socket->receiveDatagram();
    //mapped address already esablished at this point, so just disard
    if(datagram.senderAddress() == m_socket->get_current_server().address)
        return;

    handleTimer();
    QByteArray data{datagram.data()};
    quint32 action {tools::QByteArrayToInt<quint32>(data.sliced(0,4))};
    if (action == 0)
        receiveConnResponse(datagram);
    else if(action == 1)
        receiveAnnounce(datagram);
    else
        receiveMessage(datagram);
}

void DCTrackerManager::handleTimer()
{
    if(m_socket->hasPendingDatagrams())
    {
        m_trackerTimer->stop();
        m_trackerTimer->setInterval(0);
        disconnect(m_trackerTimer,&QTimer::timeout,nullptr,nullptr);
        return;
    }
    m_trackerTimer->setInterval(m_trackerTimer->interval() * 2);
    if(m_trackerTimer->interval() > 10000)
    {
        m_trackerTimer->stop();
        m_trackerTimer->setInterval(0);
        emit trackerNotResponding();
        disconnect(m_trackerTimer,&QTimer::timeout,nullptr,nullptr);
        return;
    }
}

void DCTrackerManager::connectionRequest()
{

    QByteArray data;
    QDataStream in(&data,QIODevice::ReadWrite);
    in.setByteOrder(QDataStream::BigEndian);
    in<<m_connect.request.protocol_id<<m_connect.request.action
     <<m_connect.request.transaction_id;
    qDebug()<<"bytes sent to tracker:"<<
              m_socket->writeDatagram(data,m_current_tracker.address,m_current_tracker.port);

    if(!m_trackerTimer->isActive())
    {
        connect(m_trackerTimer,&QTimer::timeout,this,&DCTrackerManager::handleTimer);
        connect(m_trackerTimer,&QTimer::timeout,this,&DCTrackerManager::connectionRequest);
        connect(m_socket.get(),&QUdpSocket::readyRead,this,&DCTrackerManager::handleDatagram);
        m_trackerTimer->setInterval(1000*3);
        m_trackerTimer->start();
    }
}

bool DCTrackerManager::receiveConnResponse(QNetworkDatagram& datagram)
{
    QByteArray data{datagram.data()};
    QDataStream out(&data,QIODevice::ReadOnly);  
    out>>m_connect.response.action>>m_connect.response.transaction_id
            >>m_connect.response.connection_id;

    if(m_connect.request.transaction_id == m_connect.response.transaction_id
            && data.size() >= 16 && m_connect.response.action == 0)
    {
        qDebug()<<"Connection response is correct.";
        emit connSucceed();
        return true;
    }
    emit connRespError();
    return false;
}

void DCTrackerManager::sendAnnounce()
{
    m_announce.request.connection_id = m_connect.response.connection_id;
    QByteArray data;
    QDataStream inAn(&data,QIODevice::WriteOnly);
    inAn<<m_announce.request.connection_id<<m_announce.request.action<<m_announce.request.transaction_id;
    for(quint8 i{0};i<20;i++)
        inAn<<m_announce.request.info_hash[i];
    for(quint8 i{0};i<20;i++)
        inAn<<m_announce.request.peer_id[i];

    inAn<<m_announce.request.downloaded<<m_announce.request.left<<m_announce.request.uploaded
       <<m_announce.request.event<<m_announce.request.ipv4<<m_announce.request.key
      <<m_announce.request.num_want<<m_announce.request.port;
    qDebug()<<"Announce bytes sent:"
           <<m_socket->writeDatagram(data,m_current_tracker.address,m_current_tracker.port);
    if(!m_trackerTimer->isActive())
    {
        m_trackerTimer->setInterval(1000*3);
        m_trackerTimer->start();
        connect(m_trackerTimer,&QTimer::timeout,this,&DCTrackerManager::handleTimer);
        connect(m_trackerTimer,&QTimer::timeout,this,&DCTrackerManager::sendAnnounce);
    }
}

void DCTrackerManager::receiveAnnounce(QNetworkDatagram& datagram)
{
    qDebug()<<"Announce response recieved.";
    QByteArray data {datagram.data()};
    QDataStream out(&data,QIODevice::ReadOnly);
    out>>m_announce.response.action>>m_announce.response.transaction_id>>m_announce.response.interval
            >>m_announce.response.leechers>>m_announce.response.seeders;
    if(m_announce.response.transaction_id == m_announce.request.transaction_id
            && m_announce.response.action == 1 && data.size() >= 20)
        qDebug()<<"Announce response is correct.";
    else
        qDebug()<<"Announce response is wrong.";
    m_endpoints.clear();
    for(quint8 n{0};(n*6)<(data.size()-20);n++)
    {
        quint32 tempIP;
        quint16 tempPort;
        out>>tempIP>>tempPort;
        m_endpoints.push_back(DCEndPoint{QHostAddress(tempIP),tempPort});
    }

    m_trackerTimer->singleShot(15 * 1000,this,&DCTrackerManager::sendAnnounce);
    emit endpointsUpdate();
}

void DCTrackerManager::sendMessage()
{
    for(quint8 i{0};i<m_endpoints.size();i++)
    {
        if(m_endpoints[i].address != m_socket->m_mapped_address->address ||
                m_endpoints[i].port != m_socket->m_mapped_address->port)
            m_socket->writeDatagram(QByteArray("Hello there"),m_endpoints[i].address,m_endpoints[i].port);
    }
}

void DCTrackerManager::receiveMessage(QNetworkDatagram& datagram)
{
    qDebug()<<"sender ip:port="<<datagram.senderAddress()<<":"<<datagram.senderPort();
    qDebug()<<"message:"<<datagram.data();
}
