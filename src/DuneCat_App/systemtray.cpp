#include "systemtray.h"
#include <QMenu>
#include <QSystemTrayIcon>

SystemTray::SystemTray(QObject *parent) : QObject(parent)
{

    // Create a context menu with two items
    QMenu* tray_icon_menu = new QMenu();

    QAction* view_window = new QAction(QStringLiteral("Open DuneCat"), this);
    QAction* quit_action = new QAction(QStringLiteral("Exit"), this);
    /* to connect the signals clicks on menu items to the appropriate signals for QML.
     * */
    connect(view_window, &QAction::triggered, this, &SystemTray::show);
    connect(quit_action, &QAction::triggered, this, &SystemTray::quit);

    tray_icon_menu->addAction(view_window);
    tray_icon_menu->addAction(quit_action);

    /* Initialize the tray icon, icon set, and specify the tooltip
     * */
    QIcon icon("qrc:/DuneCat/imports/img/LaunchButton.png");
    qDebug()<<"Called";
    m_tray_icon = new QSystemTrayIcon(icon,this);
    m_tray_icon->setContextMenu(tray_icon_menu);
    m_tray_icon->show();
    m_tray_icon->setToolTip("Tray Program" "\n"
                         "Work with winimizing program to tray");


    connect(m_tray_icon,&QSystemTrayIcon::activated,
            this, &SystemTray::icon_activated_handler);
}

/* The method that handles click on the application icon in the system tray
 * */
void SystemTray::icon_activated_handler(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        // In the case of pressing the signal on the icon tray in the call signal QML layer
        emit icon_activated();
        break;
    default:
        break;
    }
}

void SystemTray::hide_tray_icon()
{
   m_tray_icon->hide();
}
