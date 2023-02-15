#ifndef STUNCLIENT_H
#define STUNCLIENT_H

#include "STUNHeader"
#include <QObject>
#include <QtNetwork>

class STUNClient : public QObject
{
    Q_OBJECT
public:
    explicit STUNClient(QObject *parent = nullptr);

    QHostAddress mapped_address;
    quint16 mapped_port;
    QHostAddress local_adress;
    quint16 local_port;
signals:

};

#endif // STUNCLIENT_H
