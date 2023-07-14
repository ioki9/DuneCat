#pragma once
#include "essentialheaders.h"
#include <QObject>
#include <QWidget>
#include <QSystemTrayIcon>

class SystemTray : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SystemTray)
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit SystemTray(QObject *parent = nullptr);
    static SystemTray* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
signals:
    void icon_activated();
    void show();
    void quit();

private slots:
    /* The slot that will accept the signal from the event click on the application icon in the system tray
     */
    void icon_activated_handler(QSystemTrayIcon::ActivationReason reason);

public slots:
    Q_INVOKABLE void set_visible(bool visible);

private:
    /* Declare the object of future applications for the tray icon*/
    QSystemTrayIcon* m_tray_icon;

};
