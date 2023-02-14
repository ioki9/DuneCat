#ifndef DCTOOLS_H
#define DCTOOLS_H
#include <QString>

namespace tools
{
    QString macOSXAppBundlePath();
    QString macOSXAppBundleName();

    bool bootUpStart(bool isOn);
}
#endif // DCTOOLS_H
