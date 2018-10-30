#include "testutil.h"

#include <log.h>

class LogTest: public QObject
{
    Q_OBJECT
private slots:
    void simpleLogTest() {
        LOG_TRACE("This is a log info with value %1%", 22);
        LOG_INFO("This is a log info");
        LOG_WARN("100 and %1% is %2%", 22, 122);
    }
};
REGISTER_TEST(LogTest);

#include "logtest.moc"
