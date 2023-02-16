#pragma once
#include <netioapi.h>
#include <QtGlobal>
#include <QtNetwork>

struct DCSTUNHeader
{
    quint16 msg_type{};
    quint16 msg_length{0};
    constexpr quint32 magic_cookie {htonl(0x2112A442)};
    quint32 transaction_id[3];
};

struct DCEndPoint
{
    QHostAddress address;
    quint16 port;
};
