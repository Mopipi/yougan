#ifndef _TCP_HANDLER_H_
#define _TCP_HANDLER_H_

#include "basehandler.h"

struct Message {
    typedef sint16 msglen;
    Message() {reset();}
    void reset() {
        read = -(msglen)sizeof(msglen);
        size = 0;
        buffer = 0;
    }
    sint32 read;
    msglen size;
    char* buffer;
};

class TcpHandler : public BaseHandler {
    DISALLOW_COPY_AND_ASSIGN(TcpHandler);
public:
    TcpHandler(NetCallback* netCallback, JobQueue *jobQueue, SOCKET sock, uint32 size);
    ~TcpHandler();
public:
    virtual void onCanRead();
    virtual void onCanWrite();
    virtual void onClose();
public:
    void send(const char *data, uint32 len);
public:
    char *m_recvBuffer;
    Message m_message;

    SpinLock m_lock;

    uint32 m_writeLen;
    uint32 m_writePoint;
    char *m_writeBuffer;

    uint32 m_pushLen;
    uint32 m_pushPoint;
    char *m_pushBuffer;
};
#endif