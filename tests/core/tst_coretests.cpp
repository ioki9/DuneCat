#include <QtTest>
#include <QCoreApplication>
#include "processtracker.h"
#include "tools.h"
#include "essentialheaders.h"
// add necessary includes here

using namespace DuneCat;
class CoreTests : public QObject
{
    Q_OBJECT

public:
    CoreTests();
    ~CoreTests();

private slots:
//windows specific tests
#ifdef Q_OS_WINDOWS
    void test_tools_fromBSTRToDateTime();
#endif
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
    ProcessTracker tracker{};
    QBENCHMARK{
        std::vector<ProcessInfo> list = tracker.get_process_list();
    }
}
//windows specific tests
#ifdef Q_OS_WINDOWS
#include <atlbase.h>
#include <charconv>
void CoreTests::test_tools_fromBSTRToDateTime()
{
    BSTR test1 = SysAllocString(L"20250912231519.818644");
    QCOMPARE(QDateTime(), fromBSTRToDateTime(test1));

    std::wstring wstr{};
    QDateTime current = QDateTime::currentDateTime();
    current -= std::chrono::milliseconds(3000);
    wstr.append(current.date().toString("yyyyMMdd").toStdWString());
    wstr.append(current.time().toString("HHmmss.zzz").toStdWString());
    BSTR test2 = SysAllocStringLen(wstr.data(),wstr.size());
    QCOMPARE(current,fromBSTRToDateTime(test2));

    SysFreeString(test1);
    SysFreeString(test2);

}
#endif //#ifdef Q_OS_WINDOWS
QTEST_MAIN(CoreTests)

#include "tst_coretests.moc"
