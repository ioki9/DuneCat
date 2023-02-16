#ifndef DCTOOLS_H
#define DCTOOLS_H

#include "EssentialHeaders.pch"

namespace tools
{
    QString macOSXAppBundlePath();
    QString macOSXAppBundleName();

    bool bootUpStart(bool isOn);
}
#endif // DCTOOLS_H
