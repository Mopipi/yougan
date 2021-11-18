#include <time.h>

#include "logmodule.h"
#include "clockmodule.h"

const static uint32 timeWheelCount = 4;

const static uint32 timeNearBit = 8;
const static uint32 timeNear = 1 << timeNearBit;
const static uint32 timeNearMask = timeNear - 1;

const static uint32 timeSlotBit = 6;
const static uint32 timeSlot = 1 << timeSlotBit;
const static uint32 timeSlotMask = timeSlot - 1;

struct TmCall {
    uint32 expire;
    TimerCall *call;
    TmCall *next;
    TmCall **prev;
};

struct TmList {
    TmCall *head;
    TmCall **tail;

    TmList() {
        clr();
    }

    TmCall* clr() {
        TmCall *tmCall = head;
        head = 0;
        tail = &head;
        return tmCall;
    }

    void swap(TmList* tmList) {
        if (head) {
            head->prev = tmList->tail;
            tmList->head = head;
            tmList->tail = tail;
            clr();
        }
    }

    void add(TmCall *call) {
        call->next = 0;
        call->prev = tail;
        *tail = call;
        tail = &call->next;
    }

    void del(TmCall *call) {
        if (call->next) {
            call->next->prev = call->prev;
        } else {
            tail = call->prev;
        }
        *call->prev = call->next;
        call->next = 0;
        call->prev = 0;
    }

};

struct Timer {
    uint32 tick;
    uint32 lastTick;
    TmList tmWork;
    TmList tmNear[timeNear];
    TmList tmSlot[timeWheelCount][timeSlot];
    Timer() :tick(0), lastTick(0) {
    }

    void add(TmCall *call) {
        uint32 expire = call->expire;
        if ((expire | timeNearMask) == (tick | timeNearMask)) {
            //LOG_INFO("========Timer===tmNear===, %d, %d, %d", expire, tick, (expire&timeNearMask));
            tmNear[expire&timeNearMask].add(call);
        } else {
            uint32 mask = timeNear << timeSlotBit;
            for (int i = 0; i < timeWheelCount; ++i) {
                if ((expire | (mask - 1)) == (tick | (mask - 1))) {
                    //LOG_INFO("========Timer===tmSlot===, %d, %d, %d, %d, %d", expire, tick, mask, (expire >> (timeNearBit + i * timeSlotBit)), i);
                    tmSlot[i][(expire >> (timeNearBit + i * timeSlotBit)) & timeSlotMask].add(call);
                    break;
                }
                mask <<= timeSlotBit;
            }
        }
    }

    void push() {
        TmList *tmList = &tmNear[tick & timeNearMask];
        if (tmList->head) {
            //LOG_INFO("========push=======, %d, %d", tick, (tick & timeNearMask));
            tmList->swap(&tmWork);
        }
    }

    void move(uint32 wheel, uint32 slot) {
        TmCall *tmCall = tmSlot[wheel][slot].clr();
        while (tmCall) {
            //LOG_INFO("=========move=======,%d, %d, %d", tick, wheel, slot);
            TmCall *next = tmCall->next;
            add(tmCall);
            tmCall = next;
        }
    }

    void shift() {
        uint32 now = ++tick;
        if (now) {
            uint32 mask = timeNear;
            uint32 time = now >> timeNearBit;
            for (uint32 i = 0; ((now & (mask - 1)) == 0); ++i) {
                uint32 slot = time & timeSlotMask;
                if (slot) {
                    move(i, slot);
                    break;
                }
                mask <<= timeSlotBit;
                time >>= timeSlotBit;
            }
        } else {
            move(3, 0);
        }
    }
};

ClockModule::ClockModule(): m_utcTime(0), m_fps(0), m_fpsMs(1000/30), m_frameCount(0), m_frameTick(0) {

}

ClockModule::~ClockModule() {

}

int ClockModule::init() {
    EXPECT_ON_INIT(LOG_MODULE);

    m_timer = new Timer;
    return Succeed;
}

int ClockModule::start() {
    m_timer->lastTick = GetTickTime();
    m_thread.start((Run)running, this);
    return Succeed;
}

int ClockModule::update() {
    uint32 nowtick = GetTickTime();
    uint32 diff = nowtick - m_frameTick;
    if (m_fpsMs <= diff) {
        m_frameCount++;
        m_frameTick = nowtick;
    }

    if (m_timer->tmWork.head) {
        m_lock.lock();
        TmList tmWork;
        m_timer->tmWork.swap(&tmWork);
        m_lock.unlock();

        while (tmWork.head) {
            TmCall *tmCall = tmWork.head;
            tmCall->call->onTimeout();
            tmWork.del(tmCall);
            delete tmCall;
        }
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

uint32 ClockModule::timerAdd(TimerCall * call, uint32 ms) {
    TmCall *tmCall = new TmCall;
    m_lock.lock();
    tmCall->call = call;
    tmCall->expire = ms + m_timer->tick;
    //LOG_INFO("======ClockModule::timerAdd=======, %d, %d, %d", ms, m_timer->tick, tmCall->expire);
    m_timer->add(tmCall);
    m_lock.unlock();
    return 0;
}

uint32 ClockModule::running(ClockModule* clockModule) {
    return clockModule->work();
}

uint32 ClockModule::work() {
    m_quit = false;

    while (!m_quit) {
        m_utcTime = (uint32)time(0);

        uint32 now = GetTickTime();
        if (now != m_timer->lastTick) {
            uint32 diff = now - m_timer->lastTick;
            for (uint32 i = 0; i < diff; ++i) {
                m_lock.lock();
                m_timer->push();
                m_timer->shift();
                m_timer->push();
                m_lock.unlock();
            }
            m_timer->lastTick = now;
        }
        msleep(10);
    }
    return 0;
}