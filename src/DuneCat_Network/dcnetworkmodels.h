#pragma once
#include <QtGlobal>
#include "networkheaders.h"

template<typename E>
struct enable_bitmask_operators{ static constexpr bool enable = false;};

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator|(E lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
    static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator&(E lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
    static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator^(E lhs,E rhs){
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator~(E lhs){
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        ~static_cast<underlying>(lhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E&>::type
operator|=(E& lhs,E rhs){
    typedef typename std::underlying_type<E>::type underlying;
    lhs=static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E&>::type
operator&=(E& lhs,E rhs){
    typedef typename std::underlying_type<E>::type underlying;
    lhs=static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E&>::type
operator^=(E& lhs,E rhs){
    typedef typename std::underlying_type<E>::type underlying;
    lhs=static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
    return lhs;
}

enum class DatagramType : quint8
{
    undefined = 1 << 1,
    stun_response = 1 << 2,
    tracker_connResponse = 1 << 3,
    tracker_announResponse = 1 << 4
};
template<> struct enable_bitmask_operators<DatagramType>
{
    static constexpr bool enable=true;
};

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
    const quint32 magic_cookie {qToBigEndian(0x2112A442)};
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

struct DCTrackerConnect
{
    struct Request{
        const quint64 protocol_id {0x41727101980};
        const quint32 action {0};//0 connect
        quint32 transaction_id;
    };
    struct Response{
        quint32 action;
        quint32 transaction_id;
        quint64 connection_id;
    };
    Request request{};
    Response response{};
};

struct DCTrackerAnnounce
{
    struct Request{
        quint64 connection_id;
        quint32 action {1}; //1 announce
        quint32 transaction_id;
        quint8 info_hash[20];
        quint8 peer_id[20];
        quint64 downloaded;
        quint64 left;
        quint64 uploaded;
        quint32 event;
        quint32 ipv4;
        quint32 key;
        quint32 num_want;
        quint16 port;
    };
    struct Response{
        quint32 action;
        quint32 transaction_id;
        quint32 interval;
        quint32 leechers;
        quint32 seeders;

    };
    Request request{};
    Response response{};
};

