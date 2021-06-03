#ifndef _NET_PACK_H_
#define _NET_PACK_H_

#include "atomic.h"
#include "thread.h"
#include "synclock.h"

#define NET_BUFF_LEN 2048

struct NetMessage {
    uint32 size;
    char* buffer;
};
class NetBuffer
{
public:
    NetBuffer(Network* network, NetCallback *netCallback, JobQueue* jobQueue, NetID netid, uint32 size = NET_BUFF_LEN);
public:
    void onRead();
    void onWrite();
    void onClose();
public:
    void push(NetMessage);
    char* getRbuffer();
    bool enable();
    void disable();
    uint32 getRLen();
    void addRLen(uint32 len);
private:
    typedef std::vector<Message*> Queue;

    NetID m_netid;
    SOCKET m_sock;

    uint32 m_rhead;
    uint32 m_rtail;
    char *m_pushRBuffer;
    char *m_workRBuffer;
    Semaphore m_rsem;

    uint32 m_wlen;
    char* m_pushWBuffer;
    char* m_workWBuffer;
    Semaphore m_wsem;

    JobQueue *m_jobQueue;
    NetCallback *m_callback;

    Network* m_network;
};

class NetPack {
    DISALLOW_COPY_AND_ASSIGN(NetPack)
public:
    enum {
        NetNone = 0x00,
        NetRecv = 0x01,
        NetSend = 0x02,
        NetShut = 0x04,
    };

    NetPack();
    ~NetPack();
public:
    void start();
    void push(NetBuffer* buffer, uint32 netEvent);
private:
    bool swap();
private:
    static uint32 running(NetPack* netpack);
    uint32 work();
public:
    typedef std::unordered_map<Message*, uint32> Queue;

    bool m_quit;
    SpinLock m_lock;
    SyncCond m_cond;
    Queue *m_pushQueue;
    Queue *m_workQueue;
    Thread m_thread;
};
#endif