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
    read_all_settings();
    connect(&m_last_alive_timer,&QTimer::timeout,this,&Settings::set_last_alive);
}

void Settings::read_all_settings()
{
    m_window_size = QSize{m_settings.value(QStringLiteral("MainWindow/width"),1024).toInt(),
                          m_settings.value(QStringLiteral("MainWindow/height"),768).toInt()};
    m_window_maximized = m_settings.value(QStringLiteral("MainWindow/maximized"),false).toBool();
    m_autostart = m_settings.value(QStringLiteral("AutoStart"),false).toBool();
}

bool Settings::set_autostart(bool is_on)
{
    emit autostart_changed();
    m_autostart = is_on;
    m_settings.setValue(QStringLiteral("AutoStart"),is_on);
    return bootUpStart(is_on);
}

bool Settings::is_autostart_on() const
{
    return m_autostart;
}

qint64 Settings::get_last_alive() const
{
    return m_last_alive;
}

bool Settings::is_window_maximized() const
{
    return m_window_maximized;
}

void Settings::set_window_maximized(bool maximized)
{
    m_window_maximized = maximized;
    m_settings.setValue(QStringLiteral("MainWindow/maximized"),maximized);

    emit window_maximized_changed();
}

int Settings::get_window_height() const
{
    return m_window_size.height();
}

void Settings::set_window_height(int height)
{
    m_window_size.setHeight(height);
    m_settings.setValue(QStringLiteral("MainWindow/height"),height);
    emit window_height_changed();
}

int Settings::get_window_width() const
{
    return m_window_size.width();
}

void Settings::set_window_width(int width)
{
    m_window_size.setWidth(width);
    m_settings.setValue(QStringLiteral("MainWindow/width"),width);
    emit window_width_changed();
}

void Settings::set_last_alive()
{
    m_last_alive = QDateTime::currentSecsSinceEpoch();
    m_settings.setValue(QStringLiteral("LastAlive"),m_last_alive);
    emit last_alive_changed();
}

}
