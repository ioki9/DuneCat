
#include "idgenerator.h"
#include <chrono>
namespace DuneCat
{
using namespace std::chrono;
using namespace std;
IdGenerator::IdGenerator(QObject *parent)
    : QObject{parent}
{

}


unsigned int IdGenerator::generate() const
{
    unsigned int timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    while(m_last_returned == timestamp)
        timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    m_last_returned.storeRelaxed(timestamp);
    return timestamp;
}
}
