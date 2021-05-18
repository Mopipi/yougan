#include "netevent.h"

NetEvent::NetEvent(): m_quit(true){
    m_pushHandlers = new EventHandlers;
    m_workHandlers = new EventHandlers;
}

void NetEvent::start() {
    m_thread.start((Run)running, this);
}

bool NetEvent::swapHandlers() {
    if (m_pushHandlers->empty())
        return false;

    m_lock.lock();
    EventHandlers *handlers = m_pushHandlers;
    m_pushHandlers = m_workHandlers;
    m_workHandlers = handlers;
    m_lock.unlock();
    return true;
}

void NetEvent::pushHandlers(BaseHandler *handler, uint32 ioevent) {
    m_lock.lock();
    EventHandlers::iterator iter = m_pushHandlers->find(handler);
    if (iter == m_pushHandlers->end()) {
        m_pushHandlers->insert(std::make_pair(handler, ioevent));
    } else {
        iter->second |= ioevent;
    }
    m_cond.signal();
    m_lock.unlock();
}

uint32 NetEvent::running(NetEvent* netEvent) {
    return netEvent->work();
}

uint32 NetEvent::work() {
    m_quit = false;
    while (!m_quit) {
        if (!swapHandlers()) {
            m_cond.wait();
            continue;
        }

        for (EventHandlers::iterator iter = m_workHandlers->begin(); iter != m_workHandlers->end(); ++iter) {
            BaseHandler* handler = iter->first;

            uint32 ioevent = iter->second;
            if (ioevent & iorecv) {
                handler->onCanRead();
            }
            if (ioevent & iosend) {
                handler->onCanWrite();
            }
            if (ioevent & ioshut) {
                handler->onClose();
            }
        }
        m_workHandlers->clear();
    }
    return 0;
}