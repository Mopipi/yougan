#include "network.h"
#include "tcphandler.h"

struct NetPoll {
    NetPoll() {
        m_maxfd = -1;
        FD_ZERO(&m_rfdset);
        FD_ZERO(&m_wfdset);
    }
    void add(SOCKET sock) {
        FD_SET(sock, &m_rfdset);
        m_maxfd = ((m_maxfd) > (int)(sock)) ? (m_maxfd) : (int)(sock);
    }

    void del(SOCKET sock) {
        FD_CLR(sock, &m_rfdset);
        FD_CLR(sock, &m_wfdset);

        if (sock == m_maxfd) {
            m_maxfd = -1;
            for (uint32 i = 0; i, i < m_rfdset.fd_count; ++i) {
                sock = m_rfdset.fd_array[i];
                if ((int)sock > m_maxfd) {
                    m_maxfd = sock;
                }
            }
        }
    }
    void off(NetID netid) {
        m_offLock.lock();
        m_offids.push_back(netid);
        m_offLock.unlock();
    }
#ifdef WIN32
    fd_set m_rfdset;
    fd_set m_wfdset;
    int m_maxfd;
#else
    SOCKET m_epfd;
#endif
    typedef std::vector<NetID> Offid;
    Offid m_offids;
    SpinLock m_offLock;
};

struct NetThread {
    NetThread(Network *network, NetPoll *netpoll):m_network(network), m_netpoll(netpoll) {

    }
    ~NetThread() {}
    void start() {
        m_thread.start((Run)running, this);
    }

    static uint32 running(NetThread *netThread) {
        uint32 ret = netThread->work();
        delete netThread;
        return ret;
    }
    uint32 work() {
        return m_network->dispatch(m_netpoll);
    }
private:
    NetPoll* m_netpoll;
    Network *m_network;
    Thread m_thread;
};

Network::Network() :m_quit(true) {
    Socket::startup();
}

void Network::start() {
    m_count = 2;

    m_netpolls = new NetPoll[m_count];
    for (uint32 i = 0; i < m_count; ++i) {
        NetThread* netThread = new NetThread(this, &m_netpolls[i]);
        netThread->start();
    }
}

NetID Network::addHandler(BaseHandler *handler) {
    RegisterTableItem item;
    item.handler = handler;

    m_rwlock.wlock();
    NetID netid = m_registerTable.insert(item);
    handler->setNetwork(this);
    handler->setNetId(netid);

    NetPoll *poll = &m_netpolls[netid % m_count];
    poll->add(handler->getSocket());
    m_rwlock.unwlock();
    return netid;
}

void Network::closeByNetid(NetID netid) {
    NetPoll *poll = &m_netpolls[netid % m_count];
    poll->off(netid);
}

void Network::enableWrite(BaseHandler *handler, bool enabel) {
    NetPoll *poll = &m_netpolls[handler->getNetId() % m_count];
    fd_set *wfdset = &poll->m_wfdset;
    if (enabel) {
        FD_SET(handler->getSocket(), wfdset);
    } else {
        FD_CLR(handler->getSocket(), wfdset);
    }
}

bool Network::send(NetID netid, const char *data, uint32 len) {
    bool sendok = false;
    m_rwlock.rlock();
    RegisterTableIter iter = m_registerTable.find(netid);
    if (iter != m_registerTable.end()) {
        sendok = true;
        TcpHandler *handler = (TcpHandler*)iter->handler;
        handler->send(data, len);
    }
    m_rwlock.unrlock();
    return sendok;
}

void Network::delHandler(NetPoll *poll) {
    if (poll->m_offids.size() > 0) {
        NetPoll::Offid offids;

        poll->m_offLock.lock();
        offids.swap(poll->m_offids);
        poll->m_offLock.unlock();

        m_rwlock.wlock();
        for (uint32 i = 0; i < offids.size(); ++i) {
            NetID netid = offids[i];
            RegisterTableIter iter = m_registerTable.find(netid);
            if (iter != m_registerTable.end()) {
                BaseHandler* handler = iter->handler;
                SOCKET sock = handler->getSocket();
                handler->onClose();

                poll->del(sock);
                m_registerTable.erase(netid);
            }
        }
        m_rwlock.unwlock();
    }

}

uint32 Network::dispatch(NetPoll *poll) {
    m_quit = false;

    struct timeval tv = { 1, 0 };
    fd_set rfdset;
    fd_set wfdset;
    while (!m_quit) {
        delHandler(poll);

        rfdset = poll->m_rfdset;
        wfdset = poll->m_wfdset;

        if (rfdset.fd_count == 0 && wfdset.fd_count == 0) {
            msleep(10);
            continue;
        }

        int ret = ::select(poll->m_maxfd + 1, &rfdset, &wfdset, 0, &tv);
        if (ret > 0) {
            m_rwlock.rlock();
            for (RegisterTableIter iter = m_registerTable.beg(); iter != m_registerTable.end(); ++iter) {
                BaseHandler* handler = iter->handler;
                SOCKET sock = handler->getSocket();

                if (FD_ISSET(sock, &rfdset)) {
                    handler->onCanRead();
                }
                if (FD_ISSET(sock, &wfdset)) {
                    handler->onCanWrite();
                }
            }
            m_rwlock.unrlock();
        }
        else if (ret == SOCKET_ERROR && EINTR != WSAGetLastError()) {

        }
    }
    delHandler(poll);
    return 0;
}
