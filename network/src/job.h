#ifndef _JOB_H_
#define _JOB_H_

#include "threadqueue.h"
#include "netcallback.h"

class Job {
    DISALLOW_COPY_AND_ASSIGN(Job)
public:
    Job(NetCallback *netCallBack) :m_netCallback(netCallBack) {}
    virtual ~Job() {}
    virtual void invoke() = 0;
protected:
    NetCallback * m_netCallback;
};


class JobQueue {
    DISALLOW_COPY_AND_ASSIGN(JobQueue)
public:
    JobQueue() {
        m_pushJobs = new Jobs;
        m_workJobs = new Jobs;
    }
    ~JobQueue() {
        delete m_pushJobs;
        delete m_workJobs;
    }
public:
    void invoke() {
        if (m_pushJobs->empty())
            return;

        m_lock.lock();
        Jobs *jobs = m_pushJobs;
        m_pushJobs = m_workJobs;
        m_workJobs = jobs;
        m_lock.unlock();

        for (uint32 i = 0; i < m_workJobs->size(); ++i) {
            Job *&job = m_workJobs->at(i);
            job->invoke();
            delete job;
        }
        m_workJobs->clear();
    }
public:
    void push(Job *job) {
        m_lock.lock();
        m_pushJobs->push_back(job);
        m_lock.unlock();
    }
private:
    typedef std::vector<Job*> Jobs;

    SpinLock m_lock;

    Jobs* m_pushJobs;
    Jobs* m_workJobs;
};
#endif

