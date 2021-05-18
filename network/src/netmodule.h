#ifndef _NET_MODULE_H_
#define _NET_MODULE_H_

#include "module.h"
#include "network.h"

class NetModule : public Module {
    DISALLOW_COPY_AND_ASSIGN(NetModule)
public:
    NetModule(char* moduleName = 0);
    ~NetModule() {}
public:
    virtual int init();
    virtual int start();
    virtual int update();
    virtual int stop();
    virtual int release();
public:
    void pushJob(Job *job);
public:
    NetID listen(Port port, int backlog, const char *host, NetCallback* netkCallback);
private:
    uint32 m_packSisze;
    JobQueue *m_jobQueue;
    Network *m_network;
};
#endif