#pragma once
#include "essentialheaders.h"
namespace DuneCat
{
class Settings : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Settings)
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool autostart READ is_autostart_on WRITE set_autostart NOTIFY autostart_changed)
    public:
    explicit Settings(QObject *parent = nullptr);
    bool set_autostart(bool isOn);
    [[nodiscard]] bool is_autostart_on() const;
    bool save_window_state();
signals:
    void autostart_changed();
private:
    bool m_autostart;
    QSettings m_settings;
};
}
