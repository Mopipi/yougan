#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "thread.h"
#include "raobjlist.h"
#include "basehandler.h"

struct NetPoll;

class Network{
    DISALLOW_COPY_AND_ASSIGN(Network);
public:
    Network();
    ~Network() {};
public:
    void start();
private:
    friend struct NetThread;
    uint32 dispatch(NetPoll *poll);
public:
    NetID addHandler(BaseHandler *handler);
    void closeByNetid(NetID netid);
private:
    void delHandler(NetPoll *poll);
private:
    struct RegisterTableItem
    {
        RegisterTableItem(){}
        BaseHandler *handler;
    };
    typedef RAObjList<RegisterTableItem> RegisterTable;
    typedef RAObjList<RegisterTableItem>::Iterator RegisterTableIter;

    bool m_quit;
    RegisterTable m_registerTable;
    SRWLock m_rwlock;

    uint32 m_count;

    NetPoll *m_netpolls;
};

#endif