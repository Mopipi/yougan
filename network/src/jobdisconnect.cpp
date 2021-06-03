#include "jobdisconnect.h"

JobDisconnect::JobDisconnect(NetCallback* netCallback, NetID netid)
    :Job(netCallback, netid) {

}

JobDisconnect::~JobDisconnect() {

}

void JobDisconnect::invoke() {
    m_netCallback->onDisconnect(m_netid);
}