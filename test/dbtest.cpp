#include "testutil.h"

#include <dblib/db.h>

class DbTest: public QObject
{
    Q_OBJECT
private slots:
    void connectDBTest() {
        QVERIFY(Database::connect("tcp://localhost:3306", "root", ""));

        QVERIFY(Database::setSchema("dbtest"));

    }
};
REGISTER_TEST(DbTest);

#include "dbtest.moc"
