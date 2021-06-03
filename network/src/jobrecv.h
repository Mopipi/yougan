#ifndef _JOB_RECV_H_
#define _JOB_RECV_H_

#include "job.h"

class JobRecv : public Job{
    DISALLOW_COPY_AND_ASSIGN(JobRecv);
public:
    JobRecv(NetCallback* netCallback, NetID netid, char *buffer, uint32 len);
    ~JobRecv();
public:
    virtual void invoke();
private:
    uint32 m_len;
    char *m_buffer;
};
#endif