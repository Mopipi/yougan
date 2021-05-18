#include "thread.h"

typedef struct _thread_args
{
    Run func;
    void* param;
}thread_args;

#ifdef WIN32
unsigned int __stdcall thread_proc(void* args)
{
    thread_args* th = (thread_args*)args;
    unsigned int result = (th->func)(th->param);
    free(th);
    return result;
}
#else
void* thread_proc(void* args)
{
    thread_args* th = (thread_args*)args;
    void* result = (void*)(th->fn)(th->args);
    free(th);
    return result;
}
#endif

Thread::Thread() {

}

Thread::~Thread() {

}

bool Thread::start(Run func, void* param) {
    thread_args* thread = (thread_args*)malloc(sizeof(thread_args));
    thread->func = func;
    thread->param = param;
#ifdef WIN32
    m_thread = (HANDLE)_beginthreadex(NULL, 0, thread_proc, thread, 0, NULL);
#else
    int iRet = pthread_create(&m_thread, NULL, thread_proc, thread);
    if (iRet != 0)
        return false;
#endif
    return true;
}