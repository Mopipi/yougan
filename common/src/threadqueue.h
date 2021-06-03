#ifndef _THREAD_QUEUE_H_
#define _THREAD_QUEUE_H_

#include "synclock.h"

template<class T>
class ThreadQueue {
    DISALLOW_COPY_AND_ASSIGN(ThreadQueue);
public:
    ThreadQueue(uint32 size) : m_head(0), m_tail(0), m_size(size + 1), m_rsem(0), m_wsem(size) {
        m_queue = (T*)malloc(m_size * sizeof(T));
    }

    ~ThreadQueue() {
        free(m_queue);
    }
public:
    uint32 size() {
        return (m_size + m_tail - m_head) % m_size;
    }
    void push(T& t) {
        m_wsem.wait();
        lockPush(t);
        m_rsem.post();
    }

    void pop(T* t) {
        m_rsem.wait();
        lockPop(t);
        m_wsem.post();
    }

    bool trypush(T& t, int ms = 0) {
        if (SyncSucceed != m_wsem.wait(ms)) {
            return false;
        }
        lockPush(t);
        m_rsem.post();
        return true;
    }
    bool trypop(T* t, int ms = 0) {
        if (SyncSucceed != m_rsem.wait(ms)) {
            return false;
        }
        lockPop(t);
        m_wsem.post();
        return true;
    }
private:
    void lockPush(T& t) {
        m_wlock.lock();
        new (&m_queue[m_tail++])T(t);
        if (m_tail >= m_size) {
            m_tail -= m_size;
        }
        m_wlock.unlock();
    }
    void lockPop(T* t) {
        m_rlock.lock();
        *t = m_queue[m_head];
        m_queue[m_head++].~T();
        if (m_head >= m_size) {
            m_head -= m_size;
        }
        m_rlock.unlock();
    }
private:
    T * m_queue;

    uint32 m_tail;
    uint32 m_head;
    uint32 m_size;

    SpinLock m_rlock;
    Semaphore m_rsem;

    SpinLock m_wlock;
    Semaphore m_wsem;
};
#endif