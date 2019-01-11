#ifndef THREADEDCLASS_H
#define THREADEDCLASS_H

#include <workerthread.h>

template<class EventLoop = WorkerThread>
class ThreadedClass
{
public:
    ThreadedClass(const char *threadName)
        : thread_(threadName)
    {
        thread_.CreateThread();
    }

    ~ThreadedClass()
    {
        thread_.ExitThread();
    }

   const char* threadName() const { return thread_.threadName(); }

    template<class C, typename Fnc>
    bool checkThread(C * c, Fnc fnc)
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc](){ ((*c).*fnc)(); });
        }
        return sameThread;
    }

    template<class C, typename Fnc>
    bool checkThread(const C * c, Fnc fnc) const
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc](){ ((*c).*fnc)(); });
        }
        return sameThread;
    }

    template<class C, typename Fnc, typename T>
    bool checkThread(C * c, Fnc fnc, const T & t)
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc, t](){ ((*c).*fnc)(t); });
        }
        return sameThread;
    }

    template<class C, typename Fnc, typename T>
    bool checkThread(const C * c, Fnc fnc, const T & t) const
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc, t](){ ((*c).*fnc)(t); });
        }
        return sameThread;
    }

    template<class C, typename Fnc, typename T, typename T1>
    bool checkThread(C * c, Fnc fnc, const T & t, const T1 & t1)
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc, t, t1](){ ((*c).*fnc)(t,t1); });
        }
        return sameThread;
    }

    template<class C, typename Fnc, typename T, typename T1>
    bool checkThread(const C * c, Fnc fnc, const T & t, const T1 & t1) const
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc, t, t1](){ ((*c).*fnc)(t,t1); });
        }
        return sameThread;
    }

    template<class C, typename Fnc, typename T, typename T1, typename T2>
    bool checkThread(C * c, Fnc fnc, const T & t, const T1 & t1, const T2 & t2)
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc, t, t1, t2](){ ((*c).*fnc)(t,t1,t2); });
        }
        return sameThread;
    }

    template<class C, typename Fnc, typename T, typename T1, typename T2>
    bool checkThread(const C * c, Fnc fnc, const T & t, const T1 & t1, const T2 & t2) const
    {
        const bool sameThread = isSameThread();
        if (!sameThread) {
            thread_.PostMsg([c, fnc, t, t1, t2](){ ((*c).*fnc)(t,t1,t2); });
        }
        return sameThread;
    }

private:
    bool isSameThread() const
    {
        return std::this_thread::get_id() == thread_.GetThreadId();
    }

private:
    mutable EventLoop thread_;
};

#endif // THREADEDCLASS_H
