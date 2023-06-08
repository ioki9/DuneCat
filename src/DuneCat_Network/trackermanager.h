#pragma once
#include "networkheaders.h"
#include "stunclient.h"
#include "trackermanager.h"

namespace DuneCat
{
class TrackerManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TrackerManager)
public:
    explicit TrackerManager(const EndPoint& tracker_addr,QObject* parent = nullptr);
    ~TrackerManager();
    inline EndPoint get_current_tracker() const {return m_current_tracker;}
    inline StunClient* get_socket() const { return m_socket.get();}
    inline int get_announce_interval() const {return m_reannounce_interval;}
    inline void set_announce_interval(int interval) { m_reannounce_interval = interval;}
    inline QVector<EndPoint>& get_endpoints() {return m_endpoints;}
    void open_connection(EndPoint stun_server);
    void open_connection(QVector<EndPoint> stun_servers);
    void force_announce();

signals:
    void tracker_not_responding();
    void conn_succeed();
    void conn_resp_error();
    void endpoints_updated();
private:
    std::unique_ptr<StunClient> m_socket;
    EndPoint m_current_tracker{};
    TrackerAnnounce m_announce{};
    TrackerConnect m_connect{};
    QVector<EndPoint> m_endpoints{};
    QTimer* m_timeout_timer;
    int m_reannounce_interval{10000};

    void init_parameters();
    void send_message();
    void receive_announce(QNetworkDatagram& datagram);
private slots:
    void send_announce();
    void handle_timer();
    void handle_datagram();
    void connection_request();
    void receive_message(QNetworkDatagram& datagram);
    bool receive_conn_response(QNetworkDatagram& datagram);
};
}
