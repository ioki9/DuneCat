#include <QtTest>
#include <QCoreApplication>
#include <charconv>
#include "processtracker.h"
#include "tools.h"
#include "dbmanager.h"
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
    void test_tools_fromBSTRToDateTime();
    void test_dbmanager_open_close();
    void test_dbmanager_connection();
    void test_dbmanager_copy();
    void test_dbmanager_table_exist();
    void test_processtracker_get_process_list();
};

CoreTests::CoreTests()
{

}

CoreTests::~CoreTests()
{

}

void CoreTests::test_processtracker_get_process_list()
{
    std::vector<ProcessInfo> list{};
    ProcessTracker::get_instance()->get_process_list(list);
    QVERIFY(!list.empty());
}

void CoreTests::test_dbmanager_connection()
{
    DBManager db{"test_dbmanager_connection","test.db"};
    QVERIFY(db.is_valid());
    QCOMPARE(db.get_connection_name(),QString{"test_dbmanager_connection"});
    QCOMPARE(db.get_database_name(),QString{"test.db"});

    DBManager db2;
    db2.connect("test_dbmanager_connection2",true);
    QVERIFY(db2.is_valid());
    db2.set_database_name("test.db");
    QCOMPARE(db2.get_database_name(),QString{"test.db"});
    QCOMPARE(db2.get_connection_name(),QString{"test_dbmanager_connection2"});

    db2.create_connection("test_dbmanager_connection3");
    QVERIFY(QSqlDatabase::contains("test_dbmanager_connection3"));
    db2.remove_connection("test_dbmanager_connection3");
    QVERIFY(!QSqlDatabase::contains("test_dbmanager_connection3"));

}

void CoreTests::test_dbmanager_open_close()
{
    std::unique_ptr<DBManager> db = std::make_unique<DBManager>("test_dbmanager_open_close","test.db");
    std::unique_ptr<DBManager> db2 = std::make_unique<DBManager>("test_dbmanager_open_close2","test.db");
    QSqlDatabase global = QSqlDatabase::database("test_dbmanager_open_close");

    QVERIFY(db->open());
    QVERIFY(db->is_open());
    QVERIFY(global.isOpen());
    db->close();
    QVERIFY(!db->is_open());
    QVERIFY(!global.isOpen());

    //test open connections count
    db2->connect("test_dbmanager_open_close");
    db2->open();
    db2->open();
    db->open();
    std::thread t1{&DBManager::close,db2.get()};
    t1.join();
    QVERIFY(db->is_open());
    QVERIFY(!db2->is_open());
    QVERIFY(global.isOpen());


    db2->close(); //shouldn't close it
    QVERIFY(db->is_open());
    QVERIFY(!db2->is_open());
    QVERIFY(global.isOpen());

    db->close();
    QVERIFY(!db->is_open());
    QVERIFY(!db2->is_open());
    QVERIFY(!global.isOpen());

}

void CoreTests::test_dbmanager_copy()
{
    DBManager db{"test_dbmanager_copy", "test.db"};
    db.open();
    DBManager db2(db);
    QCOMPARE(db2.get_connection_name(),QString{"test_dbmanager_copy"});
    QCOMPARE(db2.get_database_name(),QString{"test.db"});
    QSqlDatabase global = QSqlDatabase::database("test_dbmanager_copy");

    QVERIFY(db2.is_open());
    QVERIFY(db.is_open());
    QVERIFY(global.isOpen());

    db2.close();
    QVERIFY(!db2.is_open());
    QVERIFY(db.is_open());
    QVERIFY(global.isOpen());

    db.close();
    QVERIFY(!db2.is_open());
    QVERIFY(!db.is_open());
    QVERIFY(!global.isOpen());

}

void CoreTests::test_dbmanager_table_exist()
{
    if(QFile::exists(QDir::currentPath() + "/test.db"))
    {
        QFile file (QDir::currentPath()+ "/test.db");
        file.remove();
    }
    DBManager db{"test_dbmanager_table_exist","test.db"};
    db.open();
    QSqlQuery query(db.get_database());

    bool res = query.exec("CREATE TABLE my_table ("
                                                   "number integer PRIMARY KEY NOT NULL, "
                                                   "address VARCHAR(255), "
                                                   "age integer"
                                                   ");");
    qDebug()<<query.lastError().text();
    QVERIFY(res);
    QVERIFY(db.table_exists("my_table"));
    QVERIFY(!db.table_exists("no_table"));
    db.close();
}

void CoreTests::test_tools_fromBSTRToDateTime()
{
#ifndef Q_OS_WIN
    QSKIP("windows only test");
#else
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

#endif
}


QTEST_MAIN(CoreTests)

#include "tst_coretests.moc"
