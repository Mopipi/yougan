#include "loginmodule.h"

class ClientNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("ClientNetCallback onAccept %d, %d\n", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {
        uint32 a = *(uint8*)data;
        printf("%d\n", a);
    }

    virtual void onDisconnect(NetID netid) {

    }

    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) {

    }

};

class GameNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("GameNetCallback onAccept %d, %d\n", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {

    }

    virtual void onDisconnect(NetID netid) {

    }
    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) {

    }

};

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
        if (netid == m_gateid) {
            uint32 handle;
            m_netModule->connectAsyn("127.0.0.1", 8005, &handle, this);
        }
        printf("InnerNetCallback onDisconnect netid = %d\n", netid);
    }
    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) {
        if (result) {
            m_gateid = netid;
        } else {
            m_netModule->connectAsyn(host, port, &handle, this);
        }
        printf("InnerNetCallback onConnect %s\n", (result ? "true" : "false"));
    }
private:
    NetID m_gateid;
    NetModule * m_netModule;
};

LoginModule::LoginModule() {

}

LoginModule::~LoginModule() {

}

int LoginModule::init() {
    EXPECT_ON_INIT(LOG_MODULE);
    LOG_INFO("======LoginModule::init=====");
    return Succeed;
}

int LoginModule::start() {
    DEPEND_ON_START(NET_MODULE);

    m_netModule = dynamic_cast<NetModule*>(getServer()->getModel(NET_MODULE));

    ClientNetCallback *clientNetworkCallBack = new ClientNetCallback;
    m_netModule->listen("0.0.0.0", 8001, 1, clientNetworkCallBack);

    GameNetCallback *gameNetworkCallBack = new GameNetCallback;
    m_netModule->listen("0.0.0.0", 8002, 1, gameNetworkCallBack);

    InnerNetCallback *innerNetworkCallBack = new InnerNetCallback(m_netModule);
    m_netModule->listen("0.0.0.0", 8003, 1, innerNetworkCallBack);
    m_netModule->listen("0.0.0.0", 8004, 1, innerNetworkCallBack);

    //m_netModule->connect("127.0.0.1", 8004, innerNetworkCallBack);

    uint32 handle;
    m_netModule->connectAsyn("127.0.0.1", 8005, &handle, innerNetworkCallBack);
    return Succeed;
}

int LoginModule::update() {
    return Succeed;
}

int LoginModule::stop() {
    return Succeed;
}

int LoginModule::release() {
    return Succeed;
}