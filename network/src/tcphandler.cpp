#include "tcphandler.h"

#include "network.h"

#define MSG_READ_BUFF_LEN 2048

TcpHandler::TcpHandler(NetCallback* netCallback, JobQueue *jobQueue, SOCKET sock, uint32 size)
    :BaseHandler(netCallback, jobQueue, sock, HT_TCP){

}

TcpHandler::~TcpHandler() {

}

void TcpHandler::onCanRead(){
    char buff[MSG_READ_BUFF_LEN];
    while (1) {
        int ret = Socket::recv(m_sock, buff, MSG_READ_BUFF_LEN, 0);
        if (ret <= 0) {
            m_network->delHandler(m_netid);
            break;
        }
        break;
    }
}

void TcpHandler::onCanWrite() {

}

void TcpHandler::onClose() {
    Socket::shutdown(m_sock, PI_SD_BOTH);
    Socket::clostSocket(m_sock);
    delete this;
}