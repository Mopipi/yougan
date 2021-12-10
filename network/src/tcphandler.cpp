#include "tcphandler.h"

#include "network.h"
#include "jobrecv.h"
#include "jobdisconnect.h"

#define NET_BUFF_LEN 2048

TcpHandler::TcpHandler(NetCallback* netCallback, JobQueue *jobQueue, SOCKET sock, uint32 size)
    :BaseHandler(netCallback, jobQueue, sock, HT_TCP) {
    m_writeLen = 0;
    m_writePoint = 0;
    m_pushLen = 0;
    m_pushPoint = 0;
    m_recvBuffer = new char[NET_BUFF_LEN];
    m_writeBuffer = new char[NET_BUFF_LEN];
    m_pushBuffer = new char[NET_BUFF_LEN];
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
            m_network->closeByNetid(m_netid);
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
    m_lock.lock();
    while (true) {
        if (m_sock == SOCKET_ERROR) {
            return;
        }

        if (m_writeLen == 0) {
            m_lock.lock();
            char *swapBuffer = m_pushBuffer;
            m_pushBuffer = m_writeBuffer;
            m_writeBuffer = swapBuffer;

            m_writeLen = m_pushLen;
            m_pushLen = 0;
            m_lock.unlock();

            if (m_writeLen == 0) {
                m_network->enableWrite(this, false);
                return;
            }
        }

        while (m_writePoint < m_writeLen) {
            int n = Socket::send(m_sock, m_writeBuffer + m_writePoint, m_writeLen - m_writePoint, 0);
            if (n < 0) {
                if (n == SOCKET_ERROR && Socket::getErrno() == PI_EWOULDBLOCK) {
                    return;
                }
                m_network->closeByNetid(m_netid);
                return;
            }
            m_writePoint += n;
        }
        m_writeLen = 0;
        m_writePoint = 0;
    }
    m_lock.unlock();
}

void TcpHandler::onClose() {
    JobDisconnect* job = new JobDisconnect(m_netCallback, m_netid);
    m_jobQueue->push(job);

    Socket::shutdown(m_sock, PI_SD_BOTH);
    Socket::clostSocket(m_sock);
    delete this;;
}

void TcpHandler::send(const char *data, uint32 len) {
    m_lock.lock();
    char *buffer = m_pushBuffer + m_pushLen;
    Message::msglen* msglen = (Message::msglen*)buffer;
    *msglen = len;
    memcpy(buffer + sizeof(Message::msglen), data, len);
    m_pushLen = m_pushLen + sizeof(Message::msglen) + len;

    // todo :转交给网络线程处理，测试
    if (m_writeLen == 0) {
        m_network->enableWrite(this, true);
    }

    // tudo：后面在细化，优先直接发送，socket异常在转交给网络线程，还有buffer扩容。
    //if (m_writeLen == 0) {
    //    int n = Socket::send(m_sock, m_pushBuffer + m_pushPoint, m_pushLen - m_pushPoint, 0);
    //    if (n < 0) {
    //        n = 0; // ignore error, let socket thread try again
    //    }
    //    m_pushPoint += n;
    //}

    //if (m_pushPoint < m_pushLen) {
    //    m_network->enableWrite(this, true);
    //}
    //else {
    //    m_pushLen = 0;
    //    m_pushPoint = 0;
    //}
    m_lock.unlock();
}