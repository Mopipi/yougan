#ifndef _JOB_ACCEPT_H_
#define _JOB_ACCEPT_H_

#include "job.h"

class JobAccept : public Job {
    DISALLOW_COPY_AND_ASSIGN(JobAccept)
public:
    JobAccept(NetID netid, SOCKET sock, Port port, NetCallback *callback);
    ~JobAccept();
public:
    virtual void invoke();
private:
    NetID m_netid;
    SOCKET m_sock;
    Port m_listenPort;
};
#endif
