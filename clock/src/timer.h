#ifndef _TIMER_H_
#define _TIMER_H_

#include "thread.h"
#include "synclock.h"

class Timer {
    DISALLOW_COPY_AND_ASSIGN(Timer);
public:
    Timer();
    ~Timer();
public:
    void init();
    void start();
    void update(uint32 elapse);
private:
    static uint32 running(Timer* timer);
    uint32 work();
private:
    bool m_quit;
    Thread m_thread;
    SpinLock m_lock;

    uint32 m_tick;
    uint32 m_elapse; // 流逝的时间
};

#endif