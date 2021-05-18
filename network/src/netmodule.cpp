#include "netmodule.h"

#include "tcphandler.h"
#include "listenhandler.h"

NetModule::NetModule(char* moduleName) {
    m_network = new Network;
}


int NetModule::init() {
    m_packSisze = 1024;
    m_jobQueue = new JobQueue();

    m_network->init();
    return Succeed;
}

int NetModule::start() {
    m_network->start();
    return Succeed;
}

int NetModule::update() {
    m_jobQueue->invoke();
    return Pending;
}

int NetModule::stop() {
    return Succeed;
}

int NetModule::release() {
    return Succeed;
}

void NetModule::pushJob(Job *job) {

}

NetID NetModule::listen(Port port, int backlog, const char *host, NetCallback* netkCallback) {
    ListenHandler *listenhandler = new ListenHandler(netkCallback, m_jobQueue, m_packSisze);
    SOCKET sock = listenhandler->listen(port, backlog, host);
    if (sock == SOCKET_ERROR) {
        return false;
    }
    return m_network->addHandler(listenhandler);
}
