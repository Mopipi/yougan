#include "jobrecv.h"

JobRecv::JobRecv(NetCallback* netCallback, NetID netid, char *buffer, uint32 len)
    : Job(netCallback, netid), m_buffer(buffer), m_len(len) {

}

JobRecv::~JobRecv() {
}

void JobRecv::invoke() {
    m_netCallback->onRecv(m_netid, m_buffer, m_len);
}