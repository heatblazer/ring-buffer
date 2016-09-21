#ifndef CONCURENT_H
#define CONCURENT_H

#include <pthread.h>
#include <sched.h>

class Mutex;
class Thread;


/// compare and swap threads IDs to retireve
/// who has the lock
/// \brief The SpinLock class
///
class SpinLock
{
public:
    SpinLock(): m_lock(0){}
    ~SpinLock(){}
    void lock()
    {
        m_lock = pthread_self();

        while (!__sync_bool_compare_and_swap(&m_lock, m_lock, 0))
        {
            sched_yield();
        }
    }

    void unlock()
    {
        m_lock = 0;
    }

private:
    volatile pthread_t m_lock;
};


class Mutex
{
public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();


private:
    pthread_mutex_t m_mutex;
    pthread_mutexattr_t m_attr;
};


class Thread
{
    typedef void* (*thread_proc)(void*);
public:
    static pthread_t getCurrentThread();
    Thread();
    ~Thread();
    bool create(int stack_size, int priority, thread_proc cb, void* user_data);
    void join();
    void resume();
    thread_proc run;

public:
    void* m_user_data;
    pthread_t m_thread;
};

#endif // CONCURENT_H
