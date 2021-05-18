#ifndef _TCP_HANDLER_H_
#define _TCP_HANDLER_H_

#include "basehandler.h"

class TcpHandler : public BaseHandler {
    DISALLOW_COPY_AND_ASSIGN(TcpHandler)
public:
    TcpHandler(NetCallback* netCallback, JobQueue *jobQueue, SOCKET sock, uint32 size);
    ~TcpHandler();
public:
    virtual void onCanRead();
    virtual void onCanWrite();
    virtual void onClose();
};
#endif