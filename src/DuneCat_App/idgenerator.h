#pragma once
#include <QObject>
#include "essentialheaders.h"
#include <atomic>
#include <QAtomicInteger>
namespace DuneCat
{
class IdGenerator : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(IdGenerator)
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit IdGenerator(QObject *parent = nullptr);
    Q_INVOKABLE unsigned int generate() const;
signals:
private:
    mutable QAtomicInteger<quint32> m_last_returned{};
};
}
