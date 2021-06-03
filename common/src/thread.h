#ifndef _THREAD_H_
#define _THREAD_H_

#include "define.h"

#ifdef WIN32
typedef HANDLE thread_t;
#else
typedef pthread_t thread_t;
#endif

typedef uint32 (*Run)(void*);

class Thread
{
    DISALLOW_COPY_AND_ASSIGN(Thread);
public:
    Thread();
    ~Thread();
public:
    bool start(Run func, void* param);
private:
    thread_t m_thread;
};
#endif