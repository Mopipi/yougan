#ifndef _BASE_HANDLER_H_
#define _BASE_HANDLER_H_

#include "job.h"

class Network;

class BaseHandler {
    DISALLOW_COPY_AND_ASSIGN(BaseHandler)
public:
    enum HandlerType
    {
        HT_TCP,
        HT_LISTEN,
    };
    BaseHandler(NetCallback *netCallback, JobQueue *jobQueue, SOCKET sock, uint8 handlerType) 
        : m_netCallback(netCallback), m_jobQueue(jobQueue), m_sock(sock), m_handlerType(handlerType){}
    virtual ~BaseHandler() {}
public:
    virtual void onCanRead() {};
    virtual void onCanWrite() {};
    virtual void onClose() {};
public:
    void setNetwork(Network *network) {
        m_network = network;
    }

    SOCKET getSocket() {
        return m_sock;
    }
    void setNetId(NetID netid){
        m_netid = netid;
    }

    NetID getNetId() {
        return m_netid;
    }
protected:
    SOCKET m_sock;
    NetID m_netid;
    uint8 m_handlerType;
    Network *m_network;
    JobQueue *m_jobQueue;
    NetCallback* m_netCallback;
};
#endif