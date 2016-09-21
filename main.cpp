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
        m_lock.lock();
        g_ring.write(data);
        m_lock.unlock();
    }

    SpinLock m_lock;

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
    while(i < 640) {
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
    A a1, a2;

    B b;

    Thread t1, t2;
    Thread t3;

    t1.create(0, 0, cbA, &a1);
    t2.create(0,0, cbA, &a2);
    t3.create(0, 0, cbB, &b);


    t1.join();
    t2.join();
    t3.join();

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



