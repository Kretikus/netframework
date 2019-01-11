#include "testutil.h"

#include <log.h>

class T1
{
public:
    int k = 55;
    int t = 22;
    std::string s = "765765";

    std::string toString() const {
        return "k: " + std::to_string(k) +
               " t: "+ std::to_string(t) +
               " s: "+ s;
    }
};

class LogTest: public QObject
{
    Q_OBJECT
private slots:
    void simpleLogTest() {
        LOG_TRACE("This is a log info with value %1%", 22);
        LOG_INFO("This is a log info");
        LOG_WARN("100 and %1% is %2%", 22, 122);

        const std::string t1 = "tsssss";
        LOG_WARN("100 and %1% is %2%, %3%", 22, 122.025, t1);

        LOG_WARN("100 and %1% is %2%, %3%. Class: %4%", 22, 122.025, t1, T1());
    }
};
REGISTER_TEST(LogTest);

#include "logtest.moc"
