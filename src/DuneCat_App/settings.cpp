#include "settings.h"
#include "tools.h"
#include <DuneCatConfig.h>
namespace DuneCat
{
Settings::Settings(QObject *parent)
    : QObject{parent}, m_last_alive{QDateTime::currentSecsSinceEpoch()}
{
    m_settings.setAtomicSyncRequired(true);
    m_last_alive_timer.start(3000);
    connect(&m_last_alive_timer,QTimer::timeout,this,&Settings::set_last_alive);
}

bool Settings::set_autostart(bool is_on)
{
    emit autostart_changed();
    m_autostart = is_on;
    return bootUpStart(is_on);
}

bool Settings::is_autostart_on() const
{
    return m_autostart;
}

bool Settings::write_window_state()
{
}

qint64 Settings::get_last_alive() const
{
    return m_last_alive;
}

void Settings::set_last_alive()
{
    m_last_alive = QDateTime::currentSecsSinceEpoch();
    m_settings.setValue(QStringLiteral("LastAlive"),m_last_alive);
    emit last_alive_changed();
}

}
