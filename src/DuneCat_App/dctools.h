#ifndef DCTOOLS_H
#define DCTOOLS_H
#include <type_traits>
#include "essentialheaders.h"

namespace tools
{
    QString macOSXAppBundlePath();
    QString macOSXAppBundleName();
    bool bootUpStart(bool isOn);
#ifdef Q_OS_WN
    QDateTime fromBSTRToDateTime(BSTR bstr);
#endif
    template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
    inline T QByteArrayToInt(QByteArray&& bytes,QDataStream::ByteOrder orderOfBytes = QDataStream::BigEndian)
    {
        if(orderOfBytes == QDataStream::BigEndian)
            return qFromBigEndian(*(reinterpret_cast<T*>(bytes.data())));
        else
            return qFromLittleEndian(*(reinterpret_cast<T*>(bytes.data())));
    }

}
#endif // DCTOOLS_H
