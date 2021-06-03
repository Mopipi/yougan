#include "tcphandler.h"

#include "network.h"
#include "jobrecv.h"
#include "jobdisconnect.h"

#define NET_BUFF_LEN 2048

TcpHandler::TcpHandler(NetCallback* netCallback, JobQueue *jobQueue, SOCKET sock, uint32 size)
    :BaseHandler(netCallback, jobQueue, sock, HT_TCP){
    m_recvBuffer = new char[NET_BUFF_LEN];
}

TcpHandler::~TcpHandler() {

}

void TcpHandler::onCanRead(){
    while (1) {
        int ret = Socket::recv(m_sock, m_recvBuffer, NET_BUFF_LEN, 0);
        if (ret <= 0) {
            int err = Socket::getErrno();
            if (err == PI_EWOULDBLOCK) {
                break;
            }
            m_network->delHandler(m_netid);
            return;
        }

        int read = ret;
        while (read > 0) {
            if (m_message.read < 0) {
                sint32 msglen = sizeof(Message::msglen) + m_message.read;
                sint32 remain = sizeof(Message::msglen) - msglen;
                if (ret < remain) {
                    remain = ret;
                }
                memcpy((uint8*)&m_message.size + msglen, m_recvBuffer + ret - read, remain);
                read -= remain;
                m_message.read += remain;
   
                if (m_message.read < 0) {
                    break;
                }
                m_message.buffer = new char[m_message.size];
                if (read == 0) {
                    break;
                }
            }
            sint32 need = m_message.size - m_message.read;
            if (read < need) {
                need = read;
            }
            memcpy(m_message.buffer + m_message.read, m_recvBuffer + ret - read, need);
            read -= need;
            m_message.read += need;
            if (m_message.read < m_message.size) {
                break;
            }
            JobRecv* job = new JobRecv(m_netCallback, m_netid, m_message.buffer, m_message.size);
            m_jobQueue->push(job);

            m_message.reset();
        }
    }
}

void TcpHandler::onCanWrite() {

}

void TcpHandler::onClose() {
    JobDisconnect* job = new JobDisconnect(m_netCallback, m_netid);
    m_jobQueue->push(job);

    Socket::shutdown(m_sock, PI_SD_BOTH);
    Socket::clostSocket(m_sock);
    delete this;
}