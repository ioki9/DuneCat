#pragma once
#include "networkheaders.h"
#include "networkmodels.h"
namespace DuneCat
{
class StunClient : public QUdpSocket
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(StunClient)

public:
    StunClient() = delete;
    explicit StunClient(QVector<EndPoint> stun_servers, QObject *parent = nullptr);
    explicit StunClient(const EndPoint& stun_server, QObject *parent = nullptr);
    ~StunClient();
    inline quint16 get_mapped_port() const {return m_mapped_address->port;}
    inline QHostAddress get_mapped_address() const { return m_mapped_address->address;}
    inline EndPoint get_current_server() const {return m_current_server;}
private:
    QVector<EndPoint> m_stunServers;
    void prepare_data();
    void send_request();
    void wait_response();
    std::unique_ptr<EndPoint> m_mapped_address;
    void compute_RTO();
    void set_header(quint16 message_type = quint16(0x0001));
    QTimer* m_timer;
    StunHeader m_msg_header;
    QByteArray m_data;
    int m_rto{1000};
    EndPoint m_current_server;


signals:
    void processing_error();
    void updated();
private slots:
    void resend_request();
    bool process_data();
};
}
