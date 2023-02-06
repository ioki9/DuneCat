#include <qsystemdetection.h>

namespace tools
{
    #ifdef Q_OS_WIN
    bool bootUpStart(bool isOn);
    #elif defined(Q_OS_UNIX)
    bool bootUpStart(bool isOn)
    #endif
}