#include "settings.h"
#include "tools.h"
#include <DuneCatConfig.h>
namespace DuneCat
{
Settings::Settings(QObject *parent)
    : QObject{parent}
{
}

bool Settings::setAutoStart(bool isOn)
{
    emit autoStartChanged();
    m_autoStart = isOn;
    return bootUpStart(isOn);
}

bool Settings::isAutoStartOn() const
{
    return m_autoStart;
}
}
