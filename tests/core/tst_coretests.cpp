#include <QtTest>
#include <QCoreApplication>
#include "dcprocesstracker.h"
// add necessary includes here

class CoreTests : public QObject
{
    Q_OBJECT

public:
    CoreTests();
    ~CoreTests();

private slots:
    void test_case1();
    void bench_get_process_list();
};

CoreTests::CoreTests()
{

}

CoreTests::~CoreTests()
{

}

void CoreTests::bench_get_process_list()
{
    DCProcessTracker tracker{};
    QBENCHMARK{
        std::vector<DCProcessInfo> list = tracker.get_process_list();
    }
}

void CoreTests::test_case1()
{
}

QTEST_MAIN(CoreTests)

#include "tst_coretests.moc"
