#pragma once
#include "networkheaders.h"
#include "dcnetworkmodels.h"

class DCStunClient : public QUdpSocket
{
    Q_OBJECT

public:
    DCStunClient() = delete;
    explicit DCStunClient(QVector<DCEndPoint> stun_servers, QObject *parent = nullptr);
    explicit DCStunClient(const DCEndPoint& stun_server, QObject *parent = nullptr);
    ~DCStunClient();
    inline quint16 get_mapped_port() const {return m_mapped_address->port;}
    inline QHostAddress get_mapped_address() const { return m_mapped_address->address;}
    inline DCEndPoint get_current_server() const {return m_current_server;}
private:
    QVector<DCEndPoint> m_stunServers;
    void prepare_data();
    void send_request();
    void wait_response();
    std::unique_ptr<DCEndPoint> m_mapped_address;
    void compute_RTO();
    void set_header(quint16 message_type = quint16(0x0001));
    QTimer* m_timer;
    DCStunHeader m_msg_header;
    QByteArray m_data;
    int m_rto{1000};
    DCEndPoint m_current_server;

signals:
    void processing_error();
    void updated();

private slots:
    void resend_request();
    bool process_data();
};
