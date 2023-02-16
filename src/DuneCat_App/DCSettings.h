#pragma once

#include "EssentialHeaders.pch"

class DCSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool autoStart READ isAutoStartOn WRITE setAutoStart NOTIFY autoStartChanged)
public:
    explicit DCSettings(QObject *parent = nullptr);
    bool setAutoStart(bool isOn);
    bool isAutoStartOn() const;

signals:
    void autoStartChanged();
private:
    bool m_autoStart;

};

