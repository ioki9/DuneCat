#pragma once
#include <type_traits>
#include "../DuneCat_App/essentialheaders.h"

namespace DuneCat
{
    QString macOSXAppBundlePath();
    QString macOSXAppBundleName();
    bool bootUpStart(bool isOn);
#ifdef Q_OS_WIN
    QDateTime fromBSTRToDateTime(BSTR bstr);
#endif
    template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
    inline T QByteArrayToInt(QByteArray&& bytes,QDataStream::ByteOrder orderOfBytes = QDataStream::BigEndian)
    {
        if(orderOfBytes == QDataStream::BigEndian)
            return qFromBigEndian(*(reinterpret_cast<T*>(bytes.data())));

        return qFromLittleEndian(*(reinterpret_cast<T*>(bytes.data())));
    }

}
