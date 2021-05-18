#include "synclock.h"

Mutex::Mutex() {
#ifdef WIN32
    InitializeCriticalSection(&m_lock);
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init((lock), &attr);
    pthread_mutexattr_destroy(&attr);
#endif
}

Mutex::~Mutex() {
#ifdef WIN32
    DeleteCriticalSection(&m_lock);
#else
    pthread_mutex_destroy(&m_lock);
#endif
}

void Mutex::lock() {
#ifdef WIN32
    EnterCriticalSection(&m_lock);
#else
    pthread_spin_lock(&m_lock);
#endif
}

bool Mutex::trylock() {
#ifdef WIN32
    return TryEnterCriticalSection(&m_lock) != 0;
#else
    return pthread_mutex_trylock(&m_Lock) == 0;
#endif
}

void Mutex::unlock() {
#ifdef WIN32
    LeaveCriticalSection(&m_lock);
#else
    pthread_mutex_unlock(&m_lock);
#endif
}

AtomSpinLock::AtomSpinLock() {
    m_tid = 0;
    m_lock = 0;
    m_count = 0;
}

AtomSpinLock::~AtomSpinLock() {
    m_tid = 0;
    m_lock = 0;
    m_count = 0;
}

void AtomSpinLock::lock() {
    ulong tid = GetCurrentThreadId();
    if (tid == m_tid) {
        ++m_count;
    } else {
        //while (locked == __sync_bool_compare_and_swap(&m_lock, unlocked, locked)) {}
        while (__sync_lock_test_and_set(&m_lock, locked));
        m_tid = tid;
        ++m_count;
    }
}

bool AtomSpinLock::trylock() {
    ulong tid = GetCurrentThreadId();
    if (tid == m_tid) {
        ++m_count;
    }
    else {
        //if (locked == __sync_bool_compare_and_swap(&m_lock, unlocked, locked)) {
        //    return false;
        //}
        if (__sync_lock_test_and_set(&m_lock, locked)) {
            return false;
        }

        m_tid = tid;
        ++m_count;
    }
    return true;
}

void AtomSpinLock::unlock() {
    if (m_tid == GetCurrentThreadId()) {
        --m_count;
        if (!m_count) {
            m_tid = 0;
            __sync_lock_test_and_set(&m_lock, unlocked);
        }
    }
}

SpinLock::SpinLock() {
#ifdef WIN32
    InitializeCriticalSectionAndSpinCount(&m_lock, 4000);
#else
    pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
#endif
}

SpinLock::~SpinLock() {
#ifdef WIN32
    DeleteCriticalSection(&m_lock);
#else
    pthread_spin_destroy(&m_lock);
#endif
}

void SpinLock::lock() {
#ifdef WIN32
    EnterCriticalSection(&m_lock);
#else
    pthread_spin_lock(&m_lock);
#endif
}

bool SpinLock::trylock() {
#ifdef WIN32
    return TryEnterCriticalSection(&m_lock) != 0;
#else
    return pthread_spin_trylock(&m_lock) == 0;
#endif
}

void SpinLock::unlock() {
#ifdef WIN32
    LeaveCriticalSection(&m_lock);
#else
    pthread_spin_unlock(&m_lock);
#endif
}

FlagSpinLock::FlagSpinLock() {
    unlock();
}

FlagSpinLock::~FlagSpinLock() {
    unlock();
}

void FlagSpinLock::lock() {
    while (m_lock.test_and_set(std::memory_order_acquire));
}

bool FlagSpinLock::trylock() {
    return !m_lock.test_and_set(std::memory_order_acquire);
}

void FlagSpinLock::unlock() {
    m_lock.clear(std::memory_order_release);
}


SyncCond::SyncCond() {
#ifdef WIN32
    m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
    pthread_cond_init(&m_cond, NULL);
    pthread_mutex_init(&m_mutex, NULL);
#endif
}

SyncCond::~SyncCond() {
#ifdef WIN32
    CloseHandle(m_event);
#else
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
#endif
}

void SyncCond::signal() {
#ifdef WIN32
    SetEvent(m_event);
#else
    pthread_cond_signal(&m_cond);
#endif
}

int SyncCond::wait(ulong ms) {
#ifdef WIN32
    int ret = WaitForSingleObject(m_event, ms);

    if (ret == WAIT_OBJECT_0) return SyncSucceed;
    if (ret == WAIT_TIMEOUT) return SyncTimeout;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct timespec ts;
    ts.tv_sec = tv.tv_sec + ms / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + (ms % 1000) * 1000000;

    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000
    }
    pthread_mutex_lock(&m_mutex);
    int ret = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
    pthread_mutex_unlock(&m_mutex);

    if (ret == 0) return SyncSucceed;
    if (ret == ETIMEDOUT) return SyncTimeout;

#endif
    return SyncFail;
}

RWLock::RWLock() : m_lock(unlocked), m_waitRCount(0), m_waitWCount(0) {
#ifdef WIN32
    
#else
    
#endif
}

RWLock::~RWLock() {
#ifdef WIN32
    //DeleteCriticalSection(&m_lock);
#else
#endif
}
void RWLock::rlock() {
#ifdef WIN32
    bool wait = false;
    while (1) {
        m_mutex.lock();
        if (wait) {
            --m_waitRCount;
        }

        if (m_lock == unlocked) {
            m_lock = rlocked;
            ++m_rlockCount;
            m_mutex.unlock();
            break;
        } else if (m_lock == rlocked) {
            if (m_waitWCount > 0) {
                ++m_waitRCount;
                m_mutex.unlock();
                WaitForSingleObject(m_event, INFINITE);
                wait = true;
            } else {
                ++m_rlockCount;
                m_mutex.unlock();
                break;
            }
        } else if (m_lock == wlocked) {
            ++m_waitRCount;
            m_mutex.unlock();
            WaitForSingleObject(m_event, INFINITE);
            wait = true;
        }
    }
#else
#endif
}

void RWLock::wlock() {
#ifdef WIN32
    bool wait = false;
    while (1) {
        m_mutex.lock();
        if (wait) {
            --m_waitWCount;
        }

        if (m_lock == unlocked) {
            m_lock = wlocked;
            m_mutex.unlock();
            break;
        } else {
            ++m_waitWCount;
            m_mutex.unlock();
            WaitForSingleObject(m_event, INFINITE);
            wait = true;
        }
    }
#else
    
#endif
}

void RWLock::unwlock() {
#ifdef WIN32
    m_mutex.lock();
    if (m_rlockCount > 0) {
        --m_rlockCount;
        if (0 == m_rlockCount) {
            m_lock = unlocked;
            if (m_waitRCount > 0 || m_waitWCount > 0) {
                SetEvent(m_event);
            }
        }
    }else {
        m_lock = unlocked;
        if (m_waitRCount > 0 || m_waitWCount > 0) {
            SetEvent(m_event);
        }
    }
    m_mutex.unlock();
#else
#endif
}

SRWLock::SRWLock() {
#ifdef WIN32
    InitializeSRWLock(&m_lock);
#else
    pthread_rwlock_init(&m_lock, NULL);
#endif
}

SRWLock::~SRWLock() {
#ifdef WIN32
    //系统会自动清理读写锁
#else
    pthread_rwlock_destroy(&m_lock);
#endif
}

void SRWLock::rlock() {
#ifdef WIN32
    AcquireSRWLockShared(&m_lock);
#else
    pthread_rwlock_rdlock(&m_lock);
#endif
}

void SRWLock::unrlock() {
#ifdef WIN32
    ReleaseSRWLockShared(&m_lock);
#else
    unwlock();
#endif
}

void SRWLock::wlock() {
#ifdef WIN32
    AcquireSRWLockExclusive(&m_lock);
#else
    pthread_rwlock_wrlock(&m_lock);
#endif
}

void SRWLock::unwlock() {
#ifdef WIN32
    ReleaseSRWLockExclusive(&m_lock);
#else
    pthread_rwlock_unlock(&m_lock);
#endif
}

AtomRWLock::AtomRWLock(): m_rlock(0), m_wlock(0){

}

AtomRWLock::~AtomRWLock() {

}

void AtomRWLock::rlock() {
    while (1) {
        while (m_wlock) {}

        __sync_add_and_fetch(&m_rlock, 1);
        if (m_wlock) {
            __sync_sub_and_fetch(&m_rlock, 1);
        } else {
            break;
        }
    }
}

void AtomRWLock::unrlock() {
    __sync_sub_and_fetch(&m_rlock, 1);
}

void AtomRWLock::wlock() {
    while (__sync_lock_test_and_set(&m_wlock, 1)) {}
    while (m_rlock) {}
}

void AtomRWLock::unwlock() {
    __sync_lock_release(&m_wlock);
}

Semaphore::Semaphore(int initcount) {
#ifdef WIN32
    m_semaphore = CreateSemaphore(NULL, initcount, 999999, NULL);
#else
    sem_init(&m_semaphore, 0, initcount);
#endif
}

Semaphore::~Semaphore() {
#ifdef WIN32
    CloseHandle(m_semaphore);
#else
    sem_destroy(&m_semaphore);
#endif
}

int Semaphore::post(int count) {
#ifdef WIN32
    return ReleaseSemaphore(m_semaphore, count, NULL);
#else
    int i = count;
    while (count-- > 0x00) sem_post(&m_Semaphore);
    return count;
#endif
}

int Semaphore::wait(int ms) {
#ifdef WIN32
    int ret = WaitForSingleObject(m_semaphore, ms);
    if (ret == WAIT_OBJECT_0) return SyncSucceed;
    if (ret == WAIT_TIMEOUT) return SyncTimeout;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct timespec ts;
    ts.tv_sec = tv.tv_sec + ms / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + (ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000
    }
    while (true)
    {
        if (0 == sem_timedwait(&m_Semaphore, &ts))
            return SyncSucceed;

        if (errno == ETIMEDOUT) {
            return SyncTimeout;
        } else if (errno == EINTR) {
            continue;
        } else {
            break;
        }
    }
#endif
    return SyncFail;
}