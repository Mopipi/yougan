#ifndef _JOB_DISCONNECT_H_
#define _JOB_DISCONNECT_H_

#include "job.h"

class JobDisconnect : public Job {
    DISALLOW_COPY_AND_ASSIGN(JobDisconnect);
public:
    JobDisconnect(NetCallback* netCallback, NetID netid);
    ~JobDisconnect();
public:
    virtual void invoke();
};
#endif