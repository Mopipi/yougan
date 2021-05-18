#ifndef _LISTEN_HANDLER_H_
#define _LISTEN_HANDLER_H_

#include "basehandler.h"

class ListenHandler : public BaseHandler {
    DISALLOW_COPY_AND_ASSIGN(ListenHandler)
public:
    ListenHandler(NetCallback* netCallback, JobQueue* jobQueue, uint32 maxPackSize);
    ~ListenHandler() {}
public:
    SOCKET listen(Port port, int backlog, const char *host = 0);

    void onCanRead();
    void onClose();
private:
    Port m_listenPort;
    uint32 m_maxPackSize;
};

#endif // _LISTEN_HANDLER_H_
