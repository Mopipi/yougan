#include "gatemodule.h"

class InnerNetCallback : public NetCallback {
public:
    InnerNetCallback(NetModule* netModule) : m_netModule(netModule) {

    }
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        LOG_INFO("InnerNetCallback onAccept %d, %d", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {
        // eg:
        struct test {
            int a;
            int b;
        };
        test *t = (struct test*)data;
        LOG_INFO("InnerNetCallback::onRecv data { int a = %d; int b = %d}", t->a, t->b);

        t->a = 99;
        t->b = 66;
        m_netModule->send(netid, t, sizeof(test));
    }

    virtual void onDisconnect(NetID netid) {

    }
    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) {
        if (!result) {
            m_netModule->connectAsyn(host, port, &handle, this);
        }
    }
private:
    NetModule * m_netModule;
};

GateModule::GateModule() {

}

GateModule::~GateModule() {

}

int GateModule::init() {
    return Succeed;
}

int GateModule::start() {
    DEPEND_ON_START(NET_MODULE);
    m_netModule = dynamic_cast<NetModule*>(getServer()->getModel(NET_MODULE));

    InnerNetCallback *innerNetworkCallBack = new InnerNetCallback(m_netModule);
    m_netModule->listen("0.0.0.0", 8005, 1, innerNetworkCallBack);
    return Succeed;
}

int GateModule::update() {
    return Succeed;
}

int GateModule::stop() {
    return Succeed;
}

int GateModule::release() {
    return Succeed;
}