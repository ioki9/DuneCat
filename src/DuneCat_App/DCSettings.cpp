#include "DCSettings.h"
#include "DCTools.h"
#include <DuneCatConfig.h>



DCSettings::DCSettings(QObject *parent)
    : QObject{parent}
{

}

bool DCSettings::setAutoStart(bool isOn)
{
    emit autoStartChanged();
    return tools::bootUpStart(isOn);
}
bool DCSettings::isAutoStartOn() const
{
    return m_autoStart;
}

