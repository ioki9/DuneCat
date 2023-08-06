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
    Q_PROPERTY(int window_height READ get_window_height WRITE set_window_height NOTIFY window_height_changed FINAL)
    Q_PROPERTY(int window_width READ get_window_width WRITE set_window_width NOTIFY window_width_changed FINAL)
    Q_PROPERTY(bool window_maximized READ is_window_maximized WRITE set_window_maximized NOTIFY window_maximized_changed FINAL)
    public:
    explicit Settings(QObject *parent = nullptr);
    bool set_autostart(bool is_on);

    [[nodiscard]] bool is_autostart_on() const;
    [[nodiscard]] qint64 get_last_alive() const;
    [[nodiscard]] int get_window_height() const;
    [[nodiscard]] int get_window_width() const;
    [[nodiscard]] bool is_window_maximized() const;
    void set_window_height(int height);
    void set_window_width(int width);
    void set_window_maximized(bool maximized);
private:
    void read_all_settings();
signals:
    void autostart_changed();
    void last_alive_changed();
    void window_height_changed();
    void window_width_changed();
    void window_maximized_changed();

private slots:
    void set_last_alive();
private:

    std::atomic_bool m_autostart;
    std::atomic_bool m_window_maximized;
    qint64 m_last_alive;
    std::atomic_int m_window_width;
    std::atomic_int m_window_height;
    QTimer m_last_alive_timer;
    QSettings m_settings;
};
}
