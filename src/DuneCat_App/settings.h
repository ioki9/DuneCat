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

    Q_PROPERTY(bool autostart READ is_autostart_on WRITE set_autostart NOTIFY autostart_changed FINAL)
    Q_PROPERTY(qint64 last_alive READ get_last_alive NOTIFY last_alive_changed FINAL)
    public:
    explicit Settings(QObject *parent = nullptr);
    bool set_autostart(bool is_on);

    [[nodiscard]] bool is_autostart_on() const;
    bool write_window_state();
    qint64 get_last_alive() const;
signals:
    void autostart_changed();
    void last_alive_changed();
private slots:
    void set_last_alive();
private:
    bool m_autostart;
    qint64 m_last_alive;
    QTimer m_last_alive_timer;
    QSettings m_settings;
};
}
