#include "dcsettings.h"
#include "dctools.h"
#include <DuneCatConfig.h>

DCSettings::DCSettings(QObject *parent)
    : QObject{parent}
{
}

bool DCSettings::setAutoStart(bool isOn)
{
    emit autoStartChanged();
    m_autoStart = isOn;
    return tools::bootUpStart(isOn);
}

bool DCSettings::isAutoStartOn() const
{
    return m_autoStart;
}

