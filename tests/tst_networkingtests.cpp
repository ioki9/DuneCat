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
    DCStunClient socket(DCEndPoint{QHostAddress("37.139.120.14"),3478});
    QCOMPARE(socket.get_current_server().port, 3478);
}

QTEST_MAIN(NetworkingTests)

#include "tst_networkingtests.moc"
