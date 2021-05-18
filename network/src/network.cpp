#include "network.h"

Network::Network():m_quit(true), m_maxfd(0) {
    Socket::startup();

    FD_ZERO(&m_rfdset[SelectBak]);
    FD_ZERO(&m_wfdset[SelectBak]);
}

void Network::init() {
    m_netEventNum = 1;
}

void Network::start() {
    m_netEvent = new NetEvent[m_netEventNum];
    for (uint32 i = 0; i < m_netEventNum; ++i) {
        m_netEvent[i].start();
    }
    m_thread.start((Run)running, this);
}

NetEvent* Network::getNetEvent(SOCKET sock) {
    return &m_netEvent[sock % m_netEventNum];
}

NetID Network::addHandler(BaseHandler *handler) {
    RegisterTableItem item;
    item.handler = handler;

    m_rwlock.wlock();
    NetID netid = m_registerTable.insert(item);
    handler->setNetwork(this);
    handler->setNetId(netid);

    addSocket(handler->getSocket());
    m_rwlock.unwlock();
    return netid;
}

void Network::delHandler(NetID netid) {
    m_dirtyLock.lock();
    m_dirtyQueue.push_back(netid);
    m_dirtyLock.unlock();
}

void Network::addSocket(SOCKET sock) {
#ifdef WIN32
    FD_SET(sock, &m_rfdset[SelectBak]);
    m_maxfd = ((m_maxfd) > (int)(sock)) ? (m_maxfd) : (int)(sock);
#else
    SOCKET sock_fd = handler->GetSocket();
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = (void *)handler;
    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
    {
        // Ìí¼ÓÊ§°Ü
    }
#endif
}

void Network::delSocket(SOCKET sock) {
    FD_CLR(sock, &m_rfdset[SelectBak]);
    FD_CLR(sock, &m_wfdset[SelectBak]);

    if (sock == m_maxfd) {
        m_maxfd = -1;
        for (RegisterTableIter iter = m_registerTable.beg(); iter != m_registerTable.end(); ++iter) {
            sock = iter->handler->getSocket();
            if (sock > m_maxfd) {
                m_maxfd = sock;
            }
        }
    }
}

void Network::dirtySocket() {
    if (m_dirtyQueue.size() > 0) {
        DirtyQueue dirtyQueue;

        m_dirtyLock.lock();
        dirtyQueue.swap(m_dirtyQueue);
        m_dirtyLock.unlock();

        m_rwlock.wlock();
        for (uint32 i = 0; i < dirtyQueue.size(); ++i) {
            NetID netid = dirtyQueue[i];
            RegisterTableIter iter = m_registerTable.find(netid);
            if (iter != m_registerTable.end()) {
                BaseHandler* handler = iter->handler;
                SOCKET sock = handler->getSocket();

                NetEvent* netEvent = getNetEvent(sock);
                netEvent->pushHandlers(handler, NetEvent::ioshut);

                delSocket(sock);
                m_registerTable.erase(netid);
            }
        }
        m_rwlock.unwlock();
    }

}
uint32 Network::running(Network* network) {
    return network->work();
}

uint32 Network::work() {
    struct timeval tv = {1, 0};

    m_quit = false;
    while (!m_quit) {
        dirtySocket();

        if (m_registerTable.size() == 0) {
            Sleep(10);
            continue;
        }

        m_rfdset[SelectUse] = m_rfdset[SelectBak];
        m_wfdset[SelectUse] = m_wfdset[SelectBak];
        int ret = ::select(m_maxfd + 1, &m_rfdset[SelectUse], &m_wfdset[SelectUse], 0, &tv);
        if (ret > 0) {
            m_rwlock.rlock();
            for (RegisterTableIter iter = m_registerTable.beg(); iter != m_registerTable.end(); ++iter) {
                BaseHandler* handler = iter->handler;
                SOCKET sock = handler->getSocket();

                uint32 ioevent = NetEvent::ionone;
                if (FD_ISSET(sock, &m_rfdset[SelectUse])) {
                    ioevent = ioevent | NetEvent::iorecv;
                }
                if (FD_ISSET(sock, &m_wfdset[SelectUse])) {
                    ioevent = ioevent | NetEvent::iosend;
                }
                if (ioevent) {
                    NetEvent* netEvent = getNetEvent(sock);
                    netEvent->pushHandlers(handler, ioevent);
                }
            }
            m_rwlock.unrlock();
        } else if (ret == SOCKET_ERROR && EINTR != WSAGetLastError()) {

        }
    }
    dirtySocket();
    return 0;
}
