#ifndef DCTOOLS_H
#define DCTOOLS_H
#include "EssentialHeaders.pch"

namespace tools
{
    QString macOSXAppBundlePath();
    QString macOSXAppBundleName();
    bool bootUpStart(bool isOn);

    //intentionally doesn't modify bytes
    template<typename T>
    inline T QByteArrayToInt(QByteArray&& bytes,QDataStream::ByteOrder orderOfBytes = QDataStream::BigEndian)
    {
        if(orderOfBytes == QDataStream::BigEndian)
            return qFromBigEndian(*(reinterpret_cast<T*>(bytes.data())));
        else
            return qFromLittleEndian(*(reinterpret_cast<T*>(bytes.data())));
    }
}
#endif // DCTOOLS_H
