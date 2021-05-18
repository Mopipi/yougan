#ifndef _NETEVENT_H_
#define _NETEVENT_H_

#include "synclock.h"
#include "thread.h"
#include "raobjlist.h"
#include "basehandler.h"

class NetEvent {
    DISALLOW_COPY_AND_ASSIGN(NetEvent)
public:
    enum {
        ionone = 0x00,
        iorecv = 0x01,
        iosend = 0x02,
        ioshut = 0x04,
    };

    NetEvent();
    ~NetEvent(){}
public:
    void start();
public:
    bool swapHandlers();
    void pushHandlers(BaseHandler *handler, uint32 ioevent);
private:
    static uint32 running(NetEvent* netEvent);
    uint32 work();
private:
    bool m_quit;
    typedef std::unordered_map<BaseHandler*, uint32> EventHandlers;

    EventHandlers *m_pushHandlers;
    EventHandlers *m_workHandlers;

    Thread m_thread;
    SpinLock m_lock;
    SyncCond m_cond;
};
#endif