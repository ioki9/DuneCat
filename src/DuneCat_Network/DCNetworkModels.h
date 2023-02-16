#pragma once
#include <QtGlobal>
#include "NetworkHeaders.pch"

struct DCStunAttribute
{
    quint16_be type;
    quint16_be length;
    quint32_be value[];
};

struct DCStunHeader
{
    quint16_be msg_type{};
    quint16_be msg_length{0};
    //constant according to RFC5389
    const quint32_be magic_cookie {0x2112A442};
    quint32_be transaction_id[3];
    DCStunAttribute attr[];
};

struct DCEndPoint
{
    QHostAddress address;
    quint16 port;
};
