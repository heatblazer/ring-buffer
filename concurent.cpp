#include "concurent.h"

Mutex::Mutex()
{
//    pthread_mutexattr_init(&m_attr);
    pthread_mutex_init(&m_mutex, 0);
}

Mutex::~Mutex()
{
//    pthread_mutexattr_destroy(&m_attr);
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

pthread_t  Thread::getCurrentThread()
{
    pthread_t t = pthread_self();
    return t;
}

Thread::Thread()
{

}

Thread::~Thread()
{

}

bool Thread::create(int stack_size, int priority, thread_proc cb, void *user_data)
{
    if (stack_size < (64*1024)) {
        stack_size = 64 * 1024;
    }
    m_user_data = user_data;
    run = cb;
    pthread_create(&m_thread, 0, cb, user_data);
    return true;
}

void Thread::join()
{
    void* res = 0;
    pthread_join(m_thread, &res);
}

void Thread::resume()
{
   // nothing
}
