#include "DCStunClient.h"
#include "../DuneCat_App/DCTools.h"

DCStunClient::DCStunClient(QVector<DCEndPoint> stunServers, QObject *parent)
    : QUdpSocket{parent}, m_stunServers{stunServers}
{

}
DCStunClient::DCStunClient(DCEndPoint stunServer, QObject *parent)
    : QUdpSocket{parent}, m_stunServers{stunServer}
{
    setHeader();
    m_timer = new QTimer();
    m_currentServer = new DCEndPoint{stunServer.address,stunServer.port};
    prepareData();
    sendRequest();
}
DCStunClient::~DCStunClient()
{
    delete m_currentServer;   
}
//m_msgHeader should be created to call this function
void DCStunClient::prepareData()
{
    bind(m_currentServer->address,m_currentServer->port);
    QDataStream out(&m_data,QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    //header.msg_length = quint16(sizeof(header.attr));
    out<<m_msgHeader.msg_type<<m_msgHeader.msg_length<<m_msgHeader.magic_cookie;
    out<<m_msgHeader.transaction_id[0]<<m_msgHeader.transaction_id[1]<<m_msgHeader.transaction_id[2];
    //out<<header.attr.type<<header.attr.length;
}
//call when m_data is ready to be sent
void DCStunClient::sendRequest()
{
    qDebug()<<"Bytes sent: "<<writeDatagram(m_data,m_currentServer->address,m_currentServer->port);
    waitResponse();
}

void DCStunClient::resendRequest()
{
    qDebug()<<"Bytes resent: "<<writeDatagram(m_data,m_currentServer->address,m_currentServer->port);
    waitResponse();
}
void DCStunClient::waitResponse()
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(3 * 1000);
    //process
    connect(this, &QUdpSocket::readyRead, this, &DCStunClient::processData);
    //resend if timed out
    connect(m_timer, &QTimer::timeout, this, &DCStunClient::resendRequest);
    m_timer->start();
}

bool DCStunClient::processData()
{
    m_timer->stop();
    disconnect(this,&QUdpSocket::readyRead,this,&DCStunClient::processData);
    if(!bytesAvailable())
        return false;
    QNetworkDatagram datagram = receiveDatagram();
    QByteArray recData = datagram.data();
    //check if transactionID is the same
    if(recData.sliced(8,12) != m_data.sliced(8,12))
        qDebug()<<"transactionID is not the same!";

    QVector<DCStunAttribute> attributes{};
    //collect all attributes
    for(qsizetype count{20};count<recData.size();)
    {
        DCStunAttribute attribute;
        attribute.type = tools::QByteArrayToInt<quint16>(recData.sliced(count,2));
        count+=2;
        attribute.length = tools::QByteArrayToInt<quint16>(recData.sliced(count,2));
        count+=2;
        attribute.body = recData.sliced(count,static_cast<qsizetype>(attribute.length));
        count+=attribute.length;
        attributes.push_back(attribute);
    }
    //process only address for now
    for(const auto& attr : attributes)
    {
        //check for MAPPED-ADDRESS or XOR-MAPPED-ADDRESS accordingly
        if(attr.type != 0x01 || attr.type != 0x20)
            continue;

        //port always at the same position regardless of attribute type
        m_mapped_address->port = tools::QByteArrayToInt<quint16>(attr.body.sliced(2,2));
        //IPv4
        if(attr.body.sliced(0,2) == 0x01)
        {
            //MAPPED-ADDRESS
            if(attr.type == 0x01)
                m_mapped_address->address(tools::QByteArrayToInt<quint16>(attr.body.sliced(4,4)));
            //XOR-MAPPED-ADDRESS
            else
                m_mapped_address->address
                        (tools::QByteArrayToInt<quint16>(attr.body.sliced(4,4)) ^ m_msgHeader.magic_cookie);

            }

        }
        //IPv6
        else
        {
            //MAPPED-ADDRESS
            if(attr.type == 0x01)
                m_mapped_address->address(reinterpret_cast<quint8*>attr.body.sliced(4,16).data());
            //XOR-MAPPED-ADDRESS
            else
            {
                quint8* ipv6 = reinterpret_cast<quint8*>attr.body.sliced(4,16).data();
                quint8* transactionID = reinterpret_cast<uint8*>(m_msgHeader.transaction_id);
                for(quint8 b{0};b<16;b++)
                {

                }
                m_mapped_address->address();

            }
        }
    }
    qDebug()<<"bytes received:"<<recData.size();
    qDebug()<<"Type:"<<recData.sliced(20,2).toHex();
    qDebug()<<"Length:"<<recData.sliced(22,2).toHex();
    qDebug()<<"Family:"<<recData.sliced(24,2).toHex();
    quint32 ip;
    quint16 port;
    memcpy(&ip,recData.sliced(28,4),4);
    memcpy(&port,recData.sliced(26,2),2);
    QHostAddress ipv4(tools::QByteArrayToInt<quint32>(recData.sliced(28,4)));
    port = qFromBigEndian(port);
    //m_mapped_address->address = ipv4;
    //m_mapped_address->port = port;
    qDebug()<<"Port(hex):"<<recData.sliced(26,2).toHex();
    qDebug()<<"Port:"<<port;
    qDebug()<<"IPv4(hex):"<<recData.sliced(28,4).toHex();
    qDebug()<<"IPv4:"<<ipv4.toString();
    qDebug()<<"Type:"<<recData.sliced(32,2).toHex();
    qDebug()<<"Length:"<<recData.sliced(34,2).toHex();
    qDebug()<<"Source add:"<<recData.sliced(40,4).toHex();
    qDebug()<<"Type:"<<recData.sliced(44,2).toHex();
    qDebug()<<"Length:"<<recData.sliced(46,2).toHex();

    qDebug()<<"Class:"<<recData.sliced(50,1).toHex();
    qDebug()<<"Error code:"<<recData.sliced(51,1).toHex();
    qApp->exit();
    return true;
}

void DCStunClient::setHeader(quint16 message_type)
{
    QRandomGenerator generator;
    //transaction id should be random
    for(quint8 i{0}; i<3; i++)
        m_msgHeader.transaction_id[i] = generator.generate();

    m_msgHeader.msg_type = message_type;
}


