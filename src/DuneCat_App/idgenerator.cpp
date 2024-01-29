
#include "idgenerator.h"
namespace DuneCat
{
IdGenerator::IdGenerator(QObject *parent)
    : QObject{parent}
{

}


quint32 IdGenerator::generate() const
{
    return m_last_returned++;
}
}
