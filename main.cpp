#include <iostream>
#include <vector>
#include <unistd.h>

#include "concurent.h"
#include "ringbuffer.h"

#include <stdio.h>

using namespace std;

static int g_controll = 1;
#define PARALEL_TEST 1

static RingBuffer<int> g_ring(10, 10);

class A
{
public:
    A() {}
    ~A(){}
    void produce(int data)
    {
//        m_lock.lock();
        LockGuard<Mutex> lck(m_mutex);
        g_ring.write(data);
//        m_lock.unlock();
    }

    SpinLock m_lock;
    Mutex m_mutex;

};


class B
{
public:
    B(){}
    ~B(){}
    void consume()
    {
        int data=0;
        g_ring.read(&data);

        m_data.push_back(data);
    }

    void print()
    {
        for(int i=0; i < m_data.size(); i++) {
            printf("[%d]", m_data.at(i));
        }
        printf("\n");
    }

    std::vector<int> m_data;

};

static void* cbA(void* data)
{
    static int i = 0;
    A* a = (A*) data;
    while(i < 100) {
        a->produce(i);
        i++;
    }
    g_controll = 0;
}

static void* cbB(void* data)
{
    B* b = (B*) data;

    do {
        b->consume();
        g_ring.deleteme();
    } while (g_controll != 0);

}


int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;
#ifdef PARALEL_TEST

    A a[10];
    B b;

    Thread producers[10];
    Thread consumer;

    for(int i=0; i < 10; i++) {
        producers[i].create(0, 0, &cbA, &a[i]);
    }
    consumer.create(0, 0, &cbB, &b);

    for(int i=0; i < 10; i++) {
        producers[i].join();
    }

    consumer.join();

#elif  SIMPLE_TEST
    RingBuffer<int> rb(20, 20);
    int arr[20] = {0};
    for(int i=0; i < 100; i++) {
        rb.write(i);
    }

    for(int i=0; i < 20; i++) {
        rb.read(&arr[i]);
    }

    for(int i=0; i < 20; i++) {
        printf("[%d]", arr[i]);
    }
    puts("");
#else

#endif

    return 0;
}




