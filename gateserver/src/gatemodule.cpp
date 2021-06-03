#include "gatemodule.h"

class InnerNetCallback : public NetCallback {
public:
    InnerNetCallback(NetModule* netModule) : m_netModule(netModule) {

    }
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("InnerNetCallback onAccept %d, %d\n", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {

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