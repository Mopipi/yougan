#include "listenhandler.h"

#include "network.h"
#include "tcphandler.h"
#include "jobaccept.h"

ListenHandler::ListenHandler(NetCallback* netCallback, JobQueue* jobQueue, uint32 maxPackSize)
    : BaseHandler(netCallback, jobQueue, SOCKET_ERROR, HT_LISTEN), m_listenPort(0){

}

SOCKET ListenHandler::listen(Port port, int backlog, const char *host) {
    if (m_sock != SOCKET_ERROR){
        return m_sock;
    }

    SOCKET sock = Socket::createSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET){
        return SOCKET_ERROR;
    }

    if (SOCKET_ERROR == Socket::setReuseAddr(sock)){
        Socket::clostSocket(sock);
        return SOCKET_ERROR;
    }

    sockaddr_in addrin;
    memset(&addrin, 0, sizeof(addrin));
    addrin.sin_family = AF_INET;
    addrin.sin_port = htons(port);
    if (host == 0){
        addrin.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else{
        ulong addr = Socket::getHostByName(host); //inet_pton(AF_INET, host, (void *)&addr.sin_addr);
        if (addr == INADDR_NONE){
            addr = htonl(INADDR_ANY);
        }
        addrin.sin_addr.s_addr = addr;
    }

    if (SOCKET_ERROR == Socket::bind(sock, (sockaddr*)&addrin)){
        Socket::clostSocket(sock);
        return SOCKET_ERROR;
    }

    if (SOCKET_ERROR == Socket::listen(sock, backlog)){
        Socket::clostSocket(sock);
        return SOCKET_ERROR;
    }

    // 设为非阻塞
    if (SOCKET_ERROR == Socket::setNonBlocking(sock)){
        Socket::clostSocket(sock);
        return SOCKET_ERROR;
    }

    m_sock = sock;
    m_listenPort = port;

    return sock;
}

void ListenHandler::onCanRead() {
    while (true) {
        sockaddr_in addr;
        SOCKET sock = Socket::accept(m_sock, (sockaddr*)&addr);
        if (sock == INVALID_SOCKET){
            break;
        }

        // 所有类型的Job考虑用池
        TcpHandler *tcphandler = new TcpHandler(m_netCallback, m_jobQueue, sock, m_maxPackSize);
        NetID netid = m_network->addHandler(tcphandler);

        Job *job = new JobAccept(netid, sock, m_listenPort, m_netCallback);
        m_jobQueue->push(job);
    }
}

void ListenHandler::onClose() {

}