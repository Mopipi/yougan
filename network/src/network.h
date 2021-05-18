#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "netevent.h"

class Network{
    DISALLOW_COPY_AND_ASSIGN(Network)
public:
    Network();
    ~Network() {};
public:
    void init();
    void start();

    NetEvent* getNetEvent(SOCKET sock);
private:
    uint32 work();
    static uint32 running(Network* network);
public:
    NetID addHandler(BaseHandler *handler);
    void delHandler(NetID netid);
private:
    void addSocket(SOCKET sock);
    void delSocket(SOCKET sock);

    void dirtySocket();
private:
    struct RegisterTableItem
    {
        BaseHandler *handler;
        int events;

        RegisterTableItem(){}
    };
    typedef RAObjList<RegisterTableItem> RegisterTable;
    typedef RAObjList<RegisterTableItem>::Iterator RegisterTableIter;
    typedef std::vector<BaseHandler*> HandlerList;
    typedef std::vector<NetID> DirtyQueue;
#ifdef WIN32
    enum {
        SelectUse,
        SelectBak,
        SelectMax,
    };
    fd_set m_rfdset[SelectMax];
    fd_set m_wfdset[SelectMax];
    SOCKET m_maxfd;
#else
    SOCKET m_epollfd;
#endif
    DirtyQueue m_dirtyQueue;
    SpinLock m_dirtyLock;

    bool m_quit;
    RegisterTable m_registerTable;
    SRWLock m_rwlock;
    Thread m_thread;

    uint32 m_netEventNum;
    NetEvent *m_netEvent;
};

#endif