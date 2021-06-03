#include "jobaccept.h"

JobAccept::JobAccept(NetID netid, SOCKET sock, Port port, NetCallback *callback)
    :Job(callback, netid), m_sock(sock), m_listenPort(port)
{

}

JobAccept::~JobAccept() {

}

void JobAccept::invoke() {
    sockaddr_in addr = {0};
    Socket::peerName(m_sock, &addr);
    Host host = ntohl(addr.sin_addr.s_addr);
    Port port = ntohs(addr.sin_port);
    m_netCallback->onAccept(m_listenPort, m_netid, host, port);
}