#ifndef _CLOCK_MODULE_H_
#define _CLOCK_MODULE_H_

#include "server.h"

#include "timer.h"

static const char CLOCK_MODULE[] = "ClcokModule";

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
    uint32 getTickTime();
private:
    Timer m_tiemr;

    uint32 m_utcTime;

    uint32 m_fps;
    uint32 m_fpsMs;
    uint32 m_frameCount;
    uint32 m_frameTick;
};
#endif