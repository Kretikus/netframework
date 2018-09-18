#include "testutil.h"

#include <workerthread.h>

#include <chrono>
#include <iostream>

class ThreadTest: public QObject
{
    Q_OBJECT
private slots:
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
