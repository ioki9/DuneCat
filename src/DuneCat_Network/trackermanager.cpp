#include "trackermanager.h"
#include "tools.h"

namespace DuneCat
{
TrackerManager::TrackerManager(EndPoint tracker, QObject *parent)
    : QObject{parent}, m_current_tracker{std::move(tracker)}
{
    m_timeout_timer = new QTimer();
    connect(this,&TrackerManager::conn_succeed,this,&TrackerManager::send_announce);
    connect(this, &TrackerManager::endpoints_updated,this, &TrackerManager::send_message);
}

TrackerManager::~TrackerManager()
{
    m_timeout_timer->deleteLater();
}

void TrackerManager::init_parameters()
{
    m_connect.request.transaction_id = QRandomGenerator::global()->generate();
    m_announce.request.action = 1;
    m_announce.request.downloaded = m_announce.request.uploaded = m_announce.request.left = 0;
    for(quint8 i{0}; i<20;i++)
    {
        m_announce.request.info_hash[i] = static_cast<quint8>(i*346253);
        m_announce.request.peer_id[i] = static_cast<quint8>(QRandomGenerator().global()->generate());
    }
    m_announce.request.transaction_id = QRandomGenerator().global()->generate();
    m_announce.request.event = 1;
    m_announce.request.ipv4 = 0;
    m_announce.request.num_want = quint32(-1);
    m_announce.request.port = m_socket->get_mapped_port();
    m_announce.request.key = 0;
}

void TrackerManager::open_connection(const EndPoint& stun_server)
{
    m_socket = std::make_unique<StunClient>(stun_server);
    connect(m_socket.get(),&StunClient::updated,this,&TrackerManager::init_parameters);
    connect(m_socket.get(),&StunClient::updated,this,&TrackerManager::connection_request);
}

void TrackerManager::open_connection(const QVector<EndPoint>& stun_servers)
{
    m_socket = std::make_unique<StunClient>(stun_servers);
    connect(m_socket.get(),&StunClient::updated,this,&TrackerManager::init_parameters);
    connect(m_socket.get(),&StunClient::updated,this,&TrackerManager::connection_request);
}

void TrackerManager::handle_datagram()
{
    QNetworkDatagram datagram = m_socket->receiveDatagram();
    //mapped address already esablished at this point, so just disard
    if(datagram.senderAddress() == m_socket->get_current_server().address)
        return;

    handle_timer();
    if(!datagram.data().size())
        return;
    QByteArray data{datagram.data()};

    quint32 action {QByteArrayToInt<quint32>(data.sliced(0,4))};
    if (action == 0)
        receive_conn_response(datagram);
    else if(action == 1)
        receive_announce(datagram);
    else
        receive_message(datagram);
}

void TrackerManager::handle_timer()
{
    if(m_socket->hasPendingDatagrams())
    {
        m_timeout_timer->stop();
        m_timeout_timer->setInterval(0);
        disconnect(m_timeout_timer,&QTimer::timeout,nullptr,nullptr);
        return;
    }
    m_timeout_timer->setInterval(m_timeout_timer->interval() * 2);
    if(m_timeout_timer->interval() > 10000)
    {
        m_timeout_timer->stop();
        m_timeout_timer->setInterval(0);
        emit tracker_not_responding();
        disconnect(m_timeout_timer,&QTimer::timeout,nullptr,nullptr);
        return;
    }
}

void TrackerManager::connection_request()
{
    QByteArray data;
    QDataStream in(&data,QIODevice::ReadWrite);
    in.setByteOrder(QDataStream::BigEndian);
    in<<m_connect.request.protocol_id<<m_connect.request.action
       <<m_connect.request.transaction_id;
    qDebug()<<"bytes sent to tracker:"<<
        m_socket->writeDatagram(data,m_current_tracker.address,m_current_tracker.port);

    if(!m_timeout_timer->isActive())
    {
        connect(m_timeout_timer,&QTimer::timeout,this,&TrackerManager::handle_timer);
        connect(m_timeout_timer,&QTimer::timeout,this,&TrackerManager::connection_request);
        connect(m_socket.get(),&QUdpSocket::readyRead,this,&TrackerManager::handle_datagram);
        m_timeout_timer->setInterval(1000*3);
        m_timeout_timer->start();
    }
}

bool TrackerManager::receive_conn_response(QNetworkDatagram& datagram)
{
    QByteArray data{datagram.data()};
    QDataStream out(&data,QIODevice::ReadOnly);
    out>>m_connect.response.action>>m_connect.response.transaction_id
        >>m_connect.response.connection_id;

    if(m_connect.request.transaction_id == m_connect.response.transaction_id
        && data.size() >= 16 && m_connect.response.action == 0)
    {
        qDebug()<<"Connection response is correct.";
        emit conn_succeed();
        return true;
    }
    emit conn_resp_error();
    return false;
}

void TrackerManager::send_announce()
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
    if(!m_timeout_timer->isActive())
    {
        m_timeout_timer->setInterval(1000*3);
        m_timeout_timer->start();
        connect(m_timeout_timer,&QTimer::timeout,this,&TrackerManager::handle_timer);
        connect(m_timeout_timer,&QTimer::timeout,this,&TrackerManager::send_announce);
    }
}

void TrackerManager::receive_announce(QNetworkDatagram& datagram)
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
        quint32 temp_ip;
        quint16 temp_port;
        out>>temp_ip>>temp_port;
        m_endpoints.push_back(EndPoint{QHostAddress(temp_ip),temp_port});
    }
    m_timeout_timer->singleShot(m_reannounce_interval,this,&TrackerManager::send_announce);
    emit endpoints_updated();
}

void TrackerManager::force_announce()
{
    //if timer is active,then we didnt get a response from previous announce so we just retun
    if(m_timeout_timer->isActive())
        return;
    send_announce();
}

void TrackerManager::send_message()
{
    for(quint8 i{0};i<m_endpoints.size();i++)
    {
        if(m_endpoints[i].address != m_socket->get_mapped_address() ||
            m_endpoints[i].port != m_socket->get_mapped_port())
            m_socket->writeDatagram(QByteArray("Hello there"),m_endpoints[i].address,m_endpoints[i].port);
    }
}

void TrackerManager::receive_message(QNetworkDatagram& datagram)
{
    qDebug()<<"sender ip:port = "<<datagram.senderAddress().toString()<<":"<<datagram.senderPort();
    qDebug()<<"message:"<<datagram.data();
}
}
