#include "workerthread.h"

#include <cassert>
#include <chrono>
#include <iostream>

class WorkerThread::ThreadMsg
{
public:
    enum MsgType {
        MSG_POST_USER_DATA = 0,
        MSG_TIMER = 1,
        MSG_EXIT_THREAD = 2
    };

    ThreadMsg(MsgType type, const WorkerThread::CallbackFnc & fnc)
        : _type(type), _callbackFnc(fnc)
    {}

    MsgType _type;
    WorkerThread::CallbackFnc _callbackFnc;
};


WorkerThread::WorkerThread(const char *threadName)
    : m_thread(nullptr)
    , THREAD_NAME(threadName)
{}

WorkerThread::~WorkerThread()
{
    if (m_thread) {
        try {
            m_thread->join();
            delete m_thread;
        } catch(std::exception ex) {
            std::cout << "Exception when joining thread: " << ex.what() << std::endl;
        }
    }
}

bool WorkerThread::CreateThread()
{
    if (!m_thread) {
        m_thread = new std::thread(&WorkerThread::Process, this);
    }
    return true;
}

void WorkerThread::ExitThread()
{
    std::unique_lock<std::mutex> lk(m_mutex);
    m_queue.push(new ThreadMsg(ThreadMsg::MSG_EXIT_THREAD, nullptr));
    m_cv.notify_one();
}

std::thread::id WorkerThread::GetThreadId() const
{
    return thisId;
}

std::thread::id WorkerThread::GetCurrentThreadId()
{
    return std::this_thread::get_id();
}

void WorkerThread::PostMsg(const WorkerThread::CallbackFnc & data)
{
    std::unique_lock<std::mutex> lk(m_mutex);
    m_queue.push(new ThreadMsg(ThreadMsg::MSG_POST_USER_DATA, data));
    m_cv.notify_one();
}

void WorkerThread::TimerThread()
{
    using namespace std::chrono_literals;
    while (!m_timerExit)
    {
        // Sleep for 250ms then put a MSG_TIMER message into queue
        std::this_thread::sleep_for(250ms);

        ThreadMsg* threadMsg = new ThreadMsg(ThreadMsg::MSG_TIMER, nullptr);

        // Add timer msg to queue and notify worker thread
        std::unique_lock<std::mutex> lk(m_mutex);
        m_queue.push(threadMsg);
        m_cv.notify_one();
    }
}

void WorkerThread::Process()
{
    thisId = std::this_thread::get_id();
    m_timerExit = false;
    std::thread timerThread(&WorkerThread::TimerThread, this);

    while (1)
    {
        ThreadMsg* msg = 0;
        {
            // Wait for a message to be added to the queue
            std::unique_lock<std::mutex> lk(m_mutex);
            while (m_queue.empty())
                m_cv.wait(lk);

            if (m_queue.empty())
                continue;

            msg = m_queue.front();
            m_queue.pop();
        }

        switch (msg->_type)
        {
            case ThreadMsg::MSG_POST_USER_DATA:
            {
                assert((msg->_callbackFnc != nullptr));

                msg->_callbackFnc();

                // Delete dynamic data passed through message queue
                delete msg;
                break;
            }

            case ThreadMsg::MSG_TIMER:
                std::cout << "Timer expired on " << THREAD_NAME << "\n";
                delete msg;
                break;

            case ThreadMsg::MSG_EXIT_THREAD:
            {
                m_timerExit = true;
                timerThread.join();

                delete msg;
                std::unique_lock<std::mutex> lk(m_mutex);
                while (!m_queue.empty())
                {
                    msg = m_queue.front();
                    m_queue.pop();
                    delete msg;
                }

                std::cout << std::endl << "Exit thread on " << THREAD_NAME << std::endl;
                return;
            }

            default:
                assert(false);
        }
    }
}
