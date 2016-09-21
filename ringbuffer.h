#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <stdint.h>
#include <string.h>
#include <iostream>

#include "concurent.h"

// lock guard //
template <class T> class LockGuard
{
public:
    explicit LockGuard(T& mutex)
        : m_mutex(mutex)
    {
        m_mutex.lock();
    }

    ~LockGuard()
    {
        m_mutex.unlock();
    }
private:
    T& m_mutex;
};



/** Track based buffer, can read into 2D tracks
 * for optimized RW operations
 */
template <class T> class RingBuffer
{
public:
    explicit RingBuffer(int size, int tracks)
        : m_size(size), m_tracks(tracks)
    {
#ifdef HAS_TRACKS
        m_buff = new T*[m_tracks]; // n tracks
        for(int i=0; i < size; i++) {
            m_buff[i] = new T[m_size]; // allocate the tracks
        }
#else
        m_buff = new T[m_size];
#endif
        memset(m_buff, 0, 10);

        RW.read_head = RW.write_head = &m_buff[0];

        m_head = &m_buff[0];
        m_tail = &m_buff[m_size-1];

    }

    ~RingBuffer()
    {
        if (m_buff) {
            delete m_buff;
            m_buff = nullptr;
        }
    }

    void deleteme()
    {
        LockGuard<Mutex> lck(m_mutex);
        for(int i=0; i < m_size; i++) {
            std::cout << "[" << m_buff[i] << "]";
        }
    }

    void write(const T data)
    {
        LockGuard<Mutex> lck(m_mutex);
        (*RW.write_head) = data;
        _advance_write_head();
    }

    void write(const T* data, const int count)
    {
        LockGuard<Mutex> lck(m_mutex);
        for(int i=0; i < count; i++) {
            *RW.write_head = data[i];
            _advance_write_head();
        }
    }

    void read(T *read_data)
    {
        LockGuard<Mutex> lck(m_mutex);
        *read_data = *RW.read_head;
        _advance_read_head();

    }

    void read(T* read_data, const int count)
    {
        LockGuard<Mutex> lck(m_mutex);
        for(int i=0; i < count; i++) {
            read_data[i] = *RW.read_head;
            _advance_read_head();
        }
    }

private:
    void _advance_write_head()
    {

        if (RW.write_head != m_tail) {
            RW.write_head++;
        } else {
            RW.write_head = (T*)m_head;
        }
    }

    void _advance_read_head()
    {
        if (RW.read_head != m_tail) {
            RW.read_head++ ;
        } else {
            RW.read_head = (T*) m_head;
        }
    }


    /// read/write head difference
    /// negative - read head is ahead
    /// null - they are at the same point
    /// positive - write head is ahead
    /// \brief _RWDifference
    /// \return
    ///
    int _RWDifference()
    {
        return (RW.write_head - RW.read_head);
    }

    int _busy_blockR()
    {
        // [0][0][0][RW.w][0][0][0]
        // <---busy---> index
        int busy_index = (RW.read_head - m_head);
        return busy_index;
    }

    int _busy_blockW()
    {
        // [0][0][0][RW.w][0][0][0]
        // <---busy---> index
        int busy_index = (RW.write_head - m_head);
        return busy_index;
    }

private:
    struct {
        T* write_head;
        T* read_head;
    } RW;


    int m_size;
    int m_tracks;
    const T*   m_head;
    const T*   m_tail;

    // a two dimennsional array of buffers
    //  RW->readHead---> (1 track)[0][0][0][0][1][1]....
    //  RW->writeHead--> (2 track)[1][1][1][0][0][0]....
    //  RW->readHead---> (2 track)[1][1][1][0][0][0]....
    //  RW->writeHead--> (3 track)[0][0][0][1][1][1]....
    //  ...................................... etc.
    // so we don`t stick for reading and writing
    // in one dimension, a simple emulation of
    // a hdd with two heads and many tracks
#ifdef HAS_TRACKS
    T** m_buff;
#else
    T* m_buff;
#endif
    Mutex m_mutex;

};

#endif // RINGBUFFER_H
