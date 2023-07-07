#include "settings.h"
#include "tools.h"
#include <DuneCatConfig.h>
namespace DuneCat
{
Settings::Settings(QObject *parent)
    : QObject{parent}
{

}

bool Settings::set_autostart(bool isOn)
{
    emit autostart_changed();
    m_autostart = isOn;
    return bootUpStart(isOn);
}

bool Settings::is_autostart_on() const
{
    return m_autostart;
}
}
