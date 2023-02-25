#pragma once
#include <QObject>
#include <memory>
#include "networkheaders.h"
#include "dcnetworkmodels.h"

class DCStunClient : public QUdpSocket
{
    Q_OBJECT

public:
    DCStunClient() = delete;
    explicit DCStunClient(QVector<DCEndPoint> stunServers, QObject *parent = nullptr);
    explicit DCStunClient(const DCEndPoint& stunServer, QObject *parent = nullptr);
    ~DCStunClient();
    std::unique_ptr<DCEndPoint> m_mapped_address;

private:
    QVector<DCEndPoint> m_stunServers;
    void prepareData();
    void sendRequest();
    void waitResponse();
    void computeRTO();
    void setHeader(quint16 message_type = quint16(0x0001));
    QTimer* m_timer;
    DCStunHeader m_msgHeader;
    QByteArray m_data;
    int m_rto{1000};
    std::unique_ptr<DCEndPoint> m_currentServer;

signals:
    void processingError();
    void updated();
private slots:
    void resendRequest();
    bool processData();
};
