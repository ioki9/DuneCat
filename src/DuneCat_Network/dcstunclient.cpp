#include "dcstunclient.h"
#include "DCTools.h"
DCStunClient::DCStunClient(QVector<DCEndPoint> stunServers, QObject *parent)
    : QUdpSocket{parent}, m_stunServers{stunServers}
{

}

DCStunClient::DCStunClient(const DCEndPoint& stunServer, QObject *parent)
    : QUdpSocket{parent}, m_stunServers{stunServer}
{
    setHeader();
    m_timer = new QTimer();
    m_currentServer = std::make_unique<DCEndPoint>(DCEndPoint{stunServer.address,stunServer.port});
    m_mapped_address = std::make_unique<DCEndPoint>(DCEndPoint{QHostAddress(),0});
    m_rto = 1000;
    prepareData();
    sendRequest();
}

DCStunClient::~DCStunClient()
{
    m_timer->deleteLater();
}

//m_msgHeader should be created before calling this function
void DCStunClient::prepareData()
{
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
    qDebug()<<"Bytes sent: "<<writeDatagram(m_data,m_currentServer->address, m_currentServer->port);
    waitResponse();
}

void DCStunClient::resendRequest()
{
    if(m_rto > 38000)
    {
        qDebug()<<"server is not responding.";
        m_timer->stop();
    }
    m_rto = m_rto * 2 + 500;
    m_timer->setInterval(m_rto);
    qDebug()<<"Bytes resent: "<<writeDatagram(m_data,m_currentServer->address, m_currentServer->port);
}

void DCStunClient::waitResponse()
{
    m_timer->setInterval(m_rto);
    connect(this, &QUdpSocket::readyRead, this, &DCStunClient::processData);
    //resend if timed out
    connect(m_timer, &QTimer::timeout, this, &DCStunClient::resendRequest);
    m_timer->start();
}

void DCStunClient::computeRTO()
{

}

bool DCStunClient::processData()
{
    m_timer->stop();
    disconnect(this,&QUdpSocket::readyRead,this,&DCStunClient::processData);
    if(!bytesAvailable())
        return false;
    QNetworkDatagram datagram = receiveDatagram();
    QByteArray recData = datagram.data();
    //dont process error responses for now
    if(tools::QByteArrayToInt<quint16>(recData.sliced(0,2)) == 0x0111)
    {
        qDebug()<<"Server returned error.";
        emit processingError();
        return false;
    }
    //check if transactionID is the same
    if(recData.sliced(8,12) != m_data.sliced(8,12))
    {
        qDebug()<<"transactionID is not the same!";
        emit processingError();
        return false;
    }

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
        if(attr.type != 0x01 && attr.type != 0x20)
            continue;

        //port always at the same position regardless of attribute type
        m_mapped_address->port = tools::QByteArrayToInt<quint16>(attr.body.sliced(2,2));
        //IPv4
        if(tools::QByteArrayToInt<quint16>(attr.body.sliced(0,2)) == 0x0001)
        {
            //MAPPED-ADDRESS
            if(attr.type == 0x01)
                m_mapped_address->address =
                    QHostAddress(tools::QByteArrayToInt<quint32>(attr.body.sliced(4,4)));
            //XOR-MAPPED-ADDRESS
            else
            {
                m_mapped_address->address =
                    QHostAddress(qFromBigEndian(*(reinterpret_cast<quint32*>(attr.body.sliced(4,4).data()))^m_msgHeader.magic_cookie));
            }
        }
        //IPv6
        else
        {
            //MAPPED-ADDRESS
            if(attr.type == 0x01)
                m_mapped_address->address =
                        QHostAddress(reinterpret_cast<quint8*>(attr.body.sliced(4,16).data()));
            //XOR-MAPPED-ADDRESS
            else
            {
                quint8* ipv6 = reinterpret_cast<quint8*>(attr.body.sliced(4,16).data());
                quint8* magic_cookie = reinterpret_cast<quint8*>(m_msgHeader.magic_cookie);
                quint8* transactionID = reinterpret_cast<quint8*>(m_msgHeader.transaction_id);

                for(quint8 b{0};b<4;b++)
                    ipv6[b] = ipv6[b] ^ magic_cookie[b];
                for(quint8 p{4},t{0}; t<12; t++,p++)
                    ipv6[p] = ipv6[p] ^ transactionID[t];

                m_mapped_address->address = QHostAddress(ipv6);
            }
        }
        break;
    }
    m_data.clear();
    qDebug()<<m_mapped_address->address.toString();
    qDebug()<<m_mapped_address->port;
    emit updated();
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

