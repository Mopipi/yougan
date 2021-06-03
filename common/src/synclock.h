#ifndef _SYNC_LOCK_H_
#define _SYNC_LOCK_H_


#include "define.h"
#include "atomic.h"

enum Sync {
    SyncFail = -1,
    SyncSucceed,
    SyncTimeout,
};

template<class T>
class AutoLock {
    DISALLOW_COPY_AND_ASSIGN(AutoLock);
public:
    AutoLock(T& lock) : m_lock(&lock) { m_lock->lock(); }
    ~AutoLock() { m_lock->unlock(); }
private:
    T * m_lock;
};

class Mutex {
    DISALLOW_COPY_AND_ASSIGN(Mutex);
public:
    typedef AutoLock<Mutex> Auto;
public:
    Mutex();
    ~Mutex();
public:
    void lock();
    bool trylock();
    void unlock();
private:
#ifdef WIN32
    CRITICAL_SECTION m_lock;
#else
    pthread_mutex_t m_lock;
#endif
};

// 原子自旋锁
class AtomSpinLock {
    DISALLOW_COPY_AND_ASSIGN(AtomSpinLock);
public:
    typedef AutoLock<AtomSpinLock> Auto;
    enum {
        unlocked,
        locked,
    };
    AtomSpinLock();
    ~AtomSpinLock();
public:
    void lock();
    bool trylock();
    void unlock();
private:
    ulong m_tid;
    ulong m_lock;
    ulong m_count;
};

// 系统自旋锁
class SpinLock {
    DISALLOW_COPY_AND_ASSIGN(SpinLock);
public:
    typedef AutoLock<SpinLock> Auto;

    SpinLock();
    ~SpinLock();
public:
    void lock();
    bool trylock();
    void unlock();
private:
#ifdef WIN32
    CRITICAL_SECTION m_lock;
#else
    pthread_spinlock_t m_lock;
#endif
};

// 系统原子锁
class FlagSpinLock {
    DISALLOW_COPY_AND_ASSIGN(FlagSpinLock);
public:
    FlagSpinLock();
    ~FlagSpinLock();
public:
    void lock();
    bool trylock();
    void unlock();
public:
    std::atomic_flag m_lock;
};

class SyncCond {
    DISALLOW_COPY_AND_ASSIGN(SyncCond);
public:
public:
    SyncCond();
    ~SyncCond();
public:
    void signal();
    int wait(ulong ms = INFINITE);
private:
#ifdef WIN32
    HANDLE m_event;
#else
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
#endif
};

// 普通读写锁
class RWLock {
    DISALLOW_COPY_AND_ASSIGN(RWLock);
public:
    enum {
        unlocked,
        rlocked,
        wlocked,
    };
    RWLock();
    ~RWLock();
public:
    void rlock();
    void unrlock() {
        unwlock();
    }

    void wlock();
    void unwlock();
public:
#ifdef WIN32
    int m_lock;
    int m_rlockCount;
    int m_waitRCount;
    int m_waitWCount;
    HANDLE m_event;
    Mutex m_mutex;
#else
#endif
};

// 系统读写锁
class SRWLock {
    DISALLOW_COPY_AND_ASSIGN(SRWLock);
public:
    SRWLock();
    ~SRWLock();
public:
    void rlock();
    void unrlock();
    void wlock();
    void unwlock();
private:
#ifdef WIN32
    SRWLOCK m_lock;
#else
    pthread_rwlock_t m_lock;
#endif
};

// 原子读写锁
class AtomRWLock {
    DISALLOW_COPY_AND_ASSIGN(AtomRWLock);
public:
    AtomRWLock();
    ~AtomRWLock();
public:
    void rlock();
    void unrlock();
    void wlock();
    void unwlock();
private:
    ulong m_rlock;
    ulong m_wlock;
};

class Semaphore {
    DISALLOW_COPY_AND_ASSIGN(Semaphore);
public:
    Semaphore(int initcount = 0);
    ~Semaphore();
public:
    int post(int count = 1);
    int wait(int ms = INFINITE);
private:
#ifdef WIN32
    HANDLE m_semaphore;
#else
    sem_t m_semaphore;
#endif
};

#endif
