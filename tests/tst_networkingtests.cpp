#include <QtTest>
#include <QCoreApplication>
#include "DuneCat_Network/dcstunclient.h"
// add necessary includes here

class NetworkingTests : public QObject
{
    Q_OBJECT

public:
    NetworkingTests();
    ~NetworkingTests();

private slots:
    void test_case1();

};

NetworkingTests::NetworkingTests()
{

}

NetworkingTests::~NetworkingTests()
{

}

void NetworkingTests::test_case1()
{
}

QTEST_MAIN(NetworkingTests)

#include "tst_networkingtests.moc"
