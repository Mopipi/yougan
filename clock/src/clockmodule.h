#ifndef _CLOCK_MODULE_H_
#define _CLOCK_MODULE_H_

#include "server.h"
#include "thread.h"
#include "synclock.h"

static const char CLOCK_MODULE[] = "ClcokModule";

class TimerCall {
    DISALLOW_COPY_AND_ASSIGN(TimerCall);
public:
    TimerCall() {}
    virtual ~TimerCall() {}
public:
    virtual void onTimeout() = 0;
};

struct Timer;

class ClockModule : public Module {
    DISALLOW_COPY_AND_ASSIGN(ClockModule);
public:
    ClockModule();
    ~ClockModule();
public:
    virtual int init();
    virtual int start();
    virtual int update();
    virtual int stop();
    virtual int release();
public:
    uint32 getUtcTime();
public:
    uint32 timerAdd(TimerCall * call, uint32 ms);
private:
    static uint32 running(ClockModule* clockModule);
    uint32 work();
private:
    bool m_quit;
    Thread m_thread;
    SpinLock m_lock;

    uint32 m_utcTime;

    uint32 m_fps;
    uint32 m_fpsMs;
    uint32 m_frameCount;
    uint32 m_frameTick;

    // ¶¨Ê±Æ÷
    Timer *m_timer;
};
#endif