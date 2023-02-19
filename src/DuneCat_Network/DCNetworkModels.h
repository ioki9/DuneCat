#pragma once
#include <QtGlobal>
#include "NetworkHeaders.pch"

struct DCStunAttribute
{
    quint16 type;
    quint16 length;
    QByteArray body;
};

struct DCStunHeader
{
    quint16 msg_type{};
    quint16 msg_length{0};
    //constant according to RFC5389
    const quint32 magic_cookie {0x2112A442};
    quint32 transaction_id[3];
    QVector<DCStunAttribute> attributes;
};

struct DCEndPoint
{
    QHostAddress address;
    quint16 port;
};

struct DCRTO
{
    float RTO;
    float SRTT;
    float RTTVAR;
    const float alpha{0.125f};
    const float beta{0.25f};
    const quint8 k{4};
};
