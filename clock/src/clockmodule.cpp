#include <time.h>

#include "clockmodule.h"

ClockModule::ClockModule(): m_utcTime(0), m_fps(0), m_fpsMs(1000/30), m_frameCount(0), m_frameTick(0) {

}

ClockModule::~ClockModule() {

}

int ClockModule::init() {
    return Succeed;
}

int ClockModule::start() {
    m_thread.start((Run)running, this);
    return Succeed;
}

int ClockModule::update() {
    uint32 newtick = getTickTime();
    uint32 diff = newtick - m_frameTick;
    if (m_fpsMs <= diff) {
        m_frameCount++;
        m_frameTick = newtick;

        m_utcTime = (uint32)time(0);
    }
    return Pending;
}

int ClockModule::stop() {
    return Succeed;
}

int ClockModule::release() {
    return Succeed;
}

uint32 ClockModule::getUtcTime() {
    return m_utcTime;
}

uint32 ClockModule::getTickTime() {
#ifdef WIN32
    //return timeGetTime(); // 使用timeGetTime() 震动频率更小，但没必要, 只做记录
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}