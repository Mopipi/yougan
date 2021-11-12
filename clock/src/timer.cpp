#include "timer.h"
Timer::Timer() : m_quit(true), m_tick(0), m_elapse(0) {
}

Timer::~Timer() {

}

void Timer::init() {

}

void Timer::start() {
    m_thread.start((Run)running, this);
}

void Timer::update(uint32 tick) {
    m_tick = tick;
}

uint32 Timer::running(Timer* timer) {
    return timer->work();
}

uint32 Timer::work() {
    m_quit = false;
    while (!m_quit) {
        msleep(10);
    }
    return 0;
}