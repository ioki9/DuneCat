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

    Q_PROPERTY(bool autoStart READ isAutoStartOn WRITE setAutoStart NOTIFY autoStartChanged)
    public:
    explicit Settings(QObject *parent = nullptr);
    bool setAutoStart(bool isOn);
    [[nodiscard]] bool isAutoStartOn() const;
signals:
    void autoStartChanged();
private:
    bool m_autoStart;
};
}
