#include "network.h"

#include "jobrecv.h"
#include "jobdisconnect.h"

NetBuffer::NetBuffer(Network* network, NetCallback *netCallback, JobQueue* jobQueue, NetID netid, uint32 size)
    : m_network(network), m_callback(netCallback), m_jobQueue(jobQueue), m_netid(netid), m_rsem(1), m_rhead(0) {
    m_pushQueue = new Queue;
    m_workQueue = new Queue;
}

void NetBuffer::push() {

}
char* NetBuffer::getRbuffer() {
    return m_workRBuffer;
}

bool NetBuffer::enable() {
    if (m_rsem.wait(0) != SyncSucceed) {
        return false;
    }
    char* swapBuffer = m_pushRBuffer;
    m_pushRBuffer = m_workRBuffer;
    m_workRBuffer = swapBuffer;
    return true;
}

void NetBuffer::disable() {
    m_rsem.post();
}

uint32 NetBuffer::getRLen() {
    uint32 head = __sync_fetch_and_add(&m_rhead, 0);
    return (NET_BUFF_LEN + m_rtail - head) % NET_BUFF_LEN;
}

void NetBuffer::addRLen(uint32 len) {
    __sync_fetch_and_add(&m_rtail, len);
}

void NetBuffer::onRead() {
    uint32 tail = __sync_fetch_and_add(&m_rtail, 0);

    __sync_fetch_and_add(&m_rhead, tail);

    JobRecv *jobrecv = new JobRecv(m_callback, m_netid, m_workRBuffer, m_rhead);
    m_jobQueue->push(jobrecv);


    disable();
}

void NetBuffer::onWrite() {

}

void NetBuffer::onClose() {
    JobDisconnect* jobdisconnect = new JobDisconnect(m_callback, m_netid);
    m_jobQueue->push(jobdisconnect);

    m_network->delHandler(m_netid);
}

NetPack::NetPack(): m_quit(true) {
    m_pushBufferQueue = new BufferQueue;
    m_workBufferQueue = new BufferQueue;
}

NetPack::~NetPack() {

}

void NetPack::start() {
    m_thread.start((Run)running, this);
}

void NetPack::push(NetBuffer* buffer, uint32 netEvent) {
    m_lock.lock();
    BufferQueue::iterator iter = m_pushBufferQueue->find(buffer);
    if (iter == m_pushBufferQueue->end()) {
        m_pushBufferQueue->insert(std::make_pair(buffer, netEvent));
    }
    else {
        iter->second |= netEvent;
    }
    m_cond.signal();
    m_lock.unlock();
}

bool NetPack::swap() {
    if (m_pushBufferQueue->empty()) {
        return false;
    }
    m_lock.lock();
    BufferQueue *swapBufferQueue = m_pushBufferQueue;
    m_pushBufferQueue = m_workBufferQueue;
    m_workBufferQueue = swapBufferQueue;
    m_lock.unlock();
    return true;
}

uint32 NetPack::running(NetPack* netpack) {
    return netpack->work();
}

uint32 NetPack::work() {
    m_quit = false;
    while (!m_quit) {
        if (!swap()) {
            m_cond.wait();
            continue;
        }

        for (BufferQueue::iterator iter = m_workBufferQueue->begin(); iter != m_workBufferQueue->end(); ++iter) {
            NetBuffer* netbuffer = iter->first;
            uint32 netEvent = iter->second;
            if (netEvent & NetRecv) {
                netbuffer->onRead();
            }
            if (netEvent & NetSend) {
                netbuffer->onWrite();
            }
            if (netEvent & NetShut) {
                netbuffer->onClose();
            }
        }
    }
    return 0;
}