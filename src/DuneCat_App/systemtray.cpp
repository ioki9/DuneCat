#include "systemtray.h"
#include <QMenu>
#include <QSystemTrayIcon>

SystemTray::SystemTray(QObject *parent) : QObject(parent)
{

    // Create a context menu with two items
    QMenu* tray_icon_menu = new QMenu();

    QAction* view_window = new QAction(QStringLiteral("Open DuneCat"), this);
    QAction* quit_action = new QAction(QStringLiteral("Exit"), this);

    tray_icon_menu->addAction(view_window);
    tray_icon_menu->addAction(quit_action);

    connect(view_window, &QAction::triggered, this, &SystemTray::show);
    connect(quit_action, &QAction::triggered, this, &SystemTray::quit);

    QIcon icon = QIcon::fromTheme(QStringLiteral("desktop"));
    m_tray_icon = new QSystemTrayIcon(icon,this);
    m_tray_icon->setContextMenu(tray_icon_menu);
    m_tray_icon->show();
    m_tray_icon->setToolTip(QStringLiteral("DuneCat"));


    connect(m_tray_icon,&QSystemTrayIcon::activated,
            this, &SystemTray::icon_activated_handler);
}


SystemTray *SystemTray::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    SystemTray* result = new SystemTray();
    return result;
}

void SystemTray::icon_activated_handler(QSystemTrayIcon::ActivationReason reason)
{
#ifndef Q_OS_MAC
    switch (reason){
    case QSystemTrayIcon::DoubleClick:
        emit show();
        break;
    default:
        break;
    }
#endif
}

void SystemTray::set_visible(bool visible)
{
    m_tray_icon->setVisible(visible);
}
