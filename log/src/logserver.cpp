#include "logserver.h"

LogServer *g_logServer;

LogServer::LogServer(Log *log): m_log(log) {

}

LogServer::~LogServer() {

}

void LogServer::printf() {

}