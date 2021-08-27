#include "netmodule.h"

#include "tcphandler.h"
#include "listenhandler.h"

NetModule::NetModule(char* moduleName):m_exist(true), m_connectAsynQueue(16), m_connectRetQueue(16) {
    m_network = new Network;

    m_maxfd = 0;
    FD_ZERO(&m_fdset);
}


int NetModule::init() {
    m_mainLoopSem = m_server->getMainLoopSem();

    m_packSize = 1024;
    m_jobQueue = new JobQueue(m_mainLoopSem);

    return Succeed;
}

int NetModule::start() {
    m_exist = false;

    m_network->start();

    m_acceptAsynThread.start((Run)acceptRunning, this);
    m_connectAsynThread.start((Run)connectRunning, this);
    return Succeed;
}

int NetModule::update() {
    m_jobQueue->invoke();

    ConnectRet cs;
    while (m_connectRetQueue.trypop(&cs)) {
        cs.netkCallback->onConnect(cs.result, cs.handle, cs.netid, cs.host, cs.port);
    }
    return Pending;
}

int NetModule::stop() {
    return Succeed;
}

int NetModule::release() {
    return Succeed;
}

uint32 NetModule::acceptRunning(NetModule* netModule) {
    return netModule->acceptAsynWork();
}

uint32 NetModule::acceptAsynWork() {
    struct timeval tv = { 5, 0 };

    fd_set fdset;
    while (!m_exist) {
        if (m_maxfd == 0) {
            Sleep(10);
            continue;
        }

        fdset = m_fdset;
        int ret = ::select(m_maxfd + 1, &fdset, 0, 0, &tv);
        if (ret > 0) {
            for (uint32 i = 0; i < m_listenQueue.size(); ++i) {
                BaseHandler*  handler = m_listenQueue[i];
                if (FD_ISSET(handler->getSocket(), &fdset)) {
                    handler->onCanRead();
                }
            }
        }
    }
    return 0;
}

uint32 NetModule::connectRunning(NetModule* netModule) {
    return netModule->connectAsynWork();
}

uint32 NetModule::connectAsynWork() {
    ConnectAsyn asyn;

    while (!m_exist) {
        m_connectAsynQueue.pop(&asyn);
        NetID netid;
        bool result = connect(asyn.host.c_str(), asyn.port, asyn.netkCallback, &netid);

        ConnectRet ret;
        ret.handle = asyn.handle;
        ret.netid = netid;
        ret.host = Socket::getHostByName(asyn.host.c_str());
        ret.port = asyn.port;
        ret.result = result;
        ret.netkCallback = asyn.netkCallback;
        m_connectRetQueue.push(ret);
    }
    return 0;
}


bool NetModule::listen(const char *host, Port port, int backlog, NetCallback* netkCallback, NetID *netid) {
    ListenHandler *listenhandler = new ListenHandler(netkCallback, m_jobQueue, m_packSize);
    SOCKET sock = listenhandler->listen(port, backlog, host);
    if (sock == SOCKET_ERROR) {
        return false;
    }
    listenhandler->setNetwork(m_network);
    listenhandler->setNetId(m_listenQueue.size());

    FD_SET(sock, &m_fdset);
    m_maxfd = m_maxfd > sock ? m_maxfd : sock;
    m_listenQueue.push_back(listenhandler);

    if (netid) {
        *netid = listenhandler->getNetId();
    }
    return true;
}

bool NetModule::connect(const char *host, Port port, NetCallback* netkCallback, NetID* netid, uint32 timeout) {
    SOCKET sock = Socket::createSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == SOCKET_ERROR) {
        return false;
    }

    if (Socket::setNonBlocking(sock) == SOCKET_ERROR) {
        Socket::clostSocket(sock);
        return false;
    }

    ulong addr = Socket::getHostByName(host);
    if (addr == INADDR_NONE) {
        Socket::clostSocket(sock);
        return false;
    }
    sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = addr;
    remote.sin_port = htons(port);

    if (Socket::connect(sock, (struct sockaddr *)&remote) == SOCKET_ERROR) {
        struct timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        fd_set fdwrite;
        FD_ZERO(&fdwrite);
        FD_SET(sock, &fdwrite);

        int ret = ::select(sock + 1, 0, &fdwrite, 0, &tv);
        if (ret <= 0 || !FD_ISSET(sock, &fdwrite) || Socket::isSocketError(sock)) {
            Socket::clostSocket(sock);
            return false;
        }
    }

    TcpHandler *handler = new TcpHandler(netkCallback, m_jobQueue, sock, m_packSize);
    NetID outid = m_network->addHandler(handler);
    if (netid) {
        *netid = outid;
    }
    return true;
}
bool NetModule::connectAsyn(Host addr, Port port, uint32* handle, NetCallback* netkCallback, uint32 timeout) {
    in_addr inaddr;
    inaddr.s_addr = addr;
    
    char host[16];
    inet_ntop(AF_INET, &inaddr, host, sizeof(host));

    return connectAsyn(host, port, handle, netkCallback);
}

bool NetModule::connectAsyn(const char *host, Port port, uint32 *handle, NetCallback* netkCallback, uint32 timeout) {
    ulong addr = Socket::getHostByName(host);
    if (addr == INADDR_NONE) {
        return false;
    }
    m_handle++;

    if (handle != 0) {
        *handle = m_handle;
    }

    ConnectAsyn asyn;
    asyn.handle = m_handle;
    asyn.host = host;
    asyn.port = port;
    asyn.timeout = timeout;
    asyn.netkCallback = netkCallback;
    if (m_connectAsynQueue.trypush(asyn)) {
        return true;
    }
    return false;
}