#ifndef _LOG_SERVER_H_
#define _LOG_SERVER_H_

#include "log.h"

class LogServer {
    DISALLOW_COPY_AND_ASSIGN(LogServer);
public:
    LogServer(Log *log);
    ~LogServer();
public:
    void printf();
private:
    Log * m_log;
};

extern LogServer *g_logServer;
#endif