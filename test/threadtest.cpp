#include "testutil.h"

#include <workerthread.h>
#include <threadedclass.h>

#include <log.h>

#include <chrono>
#include <iostream>

#include <QDebug>

class TestThreadedClass : public ThreadedClass<>
{
public:
    TestThreadedClass() : ThreadedClass("testclass") {}

    void checkThreadTest() {
        if (!checkThread(this, &TestThreadedClass::checkThreadTest)) {
            qDebug() << "Other thread. Posting. Current thread: " << LogDetails::getThreadString().c_str();
            return;
        }
        qDebug() << "Arrived at thread: " << LogDetails::getThreadString().c_str();
    }

    void checkThreadConstTest() const {
        if (!checkThread(this, &TestThreadedClass::checkThreadConstTest)) {
            qDebug() << "Other thread. Posting. Current thread: " << LogDetails::getThreadString().c_str();
            return;
        }
        qDebug() << "Arrived at thread: " << LogDetails::getThreadString().c_str();
    }

    void checkThreadTest1Arg(int k) {
        if (!checkThread(this, &TestThreadedClass::checkThreadTest1Arg, k)) {
            qDebug() << "Other thread. Posting with 1 arg. Current thread: " << LogDetails::getThreadString().c_str();
            return;
        }
        qDebug() << "Arrived at thread: " << LogDetails::getThreadString().c_str() << k;
    }

    void checkThreadConstTestStringRefArg(const std::string & s, int k, double f) const
    {
        if (!checkThread(this, &TestThreadedClass::checkThreadConstTestStringRefArg, s, k, f)) {
            qDebug() << "Other thread. Posting with 1 arg. Current thread: " << LogDetails::getThreadString().c_str();
            return;
        }
        qDebug() << "Arrived at thread: " << LogDetails::getThreadString().c_str() << s.c_str() << k << f;
    }
};

class ThreadTest: public QObject
{
    Q_OBJECT
private slots:

    void testCheckThread()
    {
        TestThreadedClass tc;
        tc.checkThreadTest();
        tc.checkThreadTest1Arg(100);
        tc.checkThreadConstTestStringRefArg("Tada", 44, 3.5);
    }

    void test()
    {
        using namespace std::chrono_literals;
        WorkerThread workerThread1("WorkerThread1");
        WorkerThread workerThread2("WorkerThread2");

        // Create worker threads
        workerThread1.CreateThread();
        workerThread2.CreateThread();

        workerThread1.PostMsg([](){std::cout << std::endl << "Called!" << std::endl; });

        std::this_thread::sleep_for(1s);

        workerThread1.ExitThread();
        workerThread2.ExitThread();
    }

};
REGISTER_TEST(ThreadTest);

#include "threadtest.moc"
