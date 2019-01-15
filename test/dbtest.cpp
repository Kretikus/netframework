#include "testutil.h"

#include <dblib/db.h>

class DbTest: public QObject
{
    Q_OBJECT
private slots:
    void connectDBTest() {
        QVERIFY(Database::connect("localhost", "roman", "sql"));

        QVERIFY(Database::setSchema("dbtest"));
        //QVERIFY(Database::execute("create table test_table(id int, name varchar(100))"));

//        {
//            PreparedStatement statement;
//            QVERIFY(statement.prepare("insert into test_table values(5, 'hello')"));
//            QVERIFY(statement.execute());
//        }

//        {
//            PreparedStatement statement;
//            QVERIFY(statement.prepare("select id,name from test_table"));
//            QVERIFY(statement.execute());
//            int i = 0;
//            while(statement.nextResult()) {
//                ++i;
//                int id;
//                QVERIFY(statement.value("id", id));
//                QCOMPARE(id, 1);
//                std::string str;
//                QVERIFY(statement.value("name", str));
//                QCOMPARE(str, std::string("hello"));
//            }
//            QCOMPARE(i, 2);
//        }

        {
            PreparedStatement statement;
            QVERIFY(statement.prepare("select id,name from test_table where id=?"));
            QVERIFY(statement.bindValue(0, 1));
            QVERIFY(statement.execute());
            int i = 0;
            while(statement.nextResult()) {
                ++i;
                int id;
                QVERIFY(statement.value("id", id));
                QCOMPARE(id, 1);
                std::string str;
                QVERIFY(statement.value("name", str));
                QCOMPARE(str, std::string("hello"));
            }
            QCOMPARE(i, 2);
        }


        {
            PreparedStatement statement;
            QVERIFY(statement.prepare("select id,name from test_table where name=?"));
            QVERIFY(statement.bindValue(0, "hello"));
            QVERIFY(statement.execute());
            int i = 0;
            while(statement.nextResult()) {
                ++i;
                int id;
                QVERIFY(statement.value("id", id));
                QCOMPARE(id, 1);
                std::string str;
                QVERIFY(statement.value("name", str));
                QCOMPARE(str, std::string("hello"));
            }
            QCOMPARE(i, 2);
        }
    }
};
REGISTER_TEST(DbTest);

#include "dbtest.moc"
