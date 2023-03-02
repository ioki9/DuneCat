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
    std::unique_ptr<DCStunClient> m_socket;
    DCEndPoint m_current_tracker;
    DCTrackerAnnounce m_announce{};
    DCTrackerConnect m_connect{};
    QVector<DCEndPoint> m_endpoints;
    QTimer* m_trackerTimer;
    bool setupConnection();
signals:
    void trackerNotResponding();
    void connSucceed();
    void connRespError();
    void endpointsUpdate();
private:
    void initParameters();

    void sendMessage();
    void receiveAnnounce(QNetworkDatagram& datagram);
private slots:
    void sendAnnounce();
    void handleTimer();
    void handleDatagram();
    void connectionRequest();
    void receiveMessage(QNetworkDatagram& datagram);
    bool receiveConnResponse(QNetworkDatagram& datagram);
};

#endif // DCCONNECTIONMANAGER_H
