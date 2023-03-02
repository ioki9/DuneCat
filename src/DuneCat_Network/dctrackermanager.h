#ifndef DCCONNECTIONMANAGER_H
#define DCCONNECTIONMANAGER_H

#include "networkheaders.h"
#include "dcstunclient.h"
#include "dctrackermanager.h"

class DCTrackerManager : public QObject
{
    Q_OBJECT
public:
    explicit DCTrackerManager(const DCEndPoint& tracker_addr,QObject* parent = nullptr);
    ~DCTrackerManager();
    inline DCEndPoint get_current_tracker() const {return m_current_tracker;}
    inline DCStunClient* get_socket() const { return m_socket.get();}
    inline int get_announce_interval() const {return m_reannounce_interval;}
    inline void set_announce_interval(int interval) { m_reannounce_interval = interval;}
    inline QVector<DCEndPoint>& get_endpoints() const {return m_endpoints;}
    void open_connection(DCEndPoint stun_server);
    void open_connection(QVector<DCEndPoint> stun_servers);
    void force_announce();

signals:
    void tracker_not_responding();
    void conn_succeed();
    void conn_resp_error();
    void endpoints_updated();
private:
    std::unique_ptr<DCStunClient> m_socket;
    DCEndPoint m_current_tracker{};
    DCTrackerAnnounce m_announce{};
    DCTrackerConnect m_connect{};
    QVector<DCEndPoint> m_endpoints{};
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

#endif // DCCONNECTIONMANAGER_H
