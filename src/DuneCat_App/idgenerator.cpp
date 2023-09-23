
#include "idgenerator.h"
#include <random>
#include <chrono>
namespace DuneCat{
using namespace std::chrono;
using namespace std;
IdGenerator::IdGenerator(QObject *parent)
    : QObject{parent}
{

}


unsigned int IdGenerator::generate() const
{
    unsigned int timestamp =  duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> distrib(1, 1000000);
    timestamp += distrib(gen);
    return timestamp;
}
}
