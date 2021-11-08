#include "loginmodule.h"

// eg:
Log *g_money = 0;

class ClientNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        LOG_INFO("ClientNetCallback onAccept %d, %d", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {
        uint32 a = *(uint8*)data;
        LOG_INFO("%d", a);
    }

    virtual void onDisconnect(NetID netid) {

    }

    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) {

    }

};

class GameNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("GameNetCallback onAccept %d, %d", host, port);
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
        printf("InnerNetCallback onAccept %d, %d", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {

    }

    virtual void onDisconnect(NetID netid) {
        if (netid == m_gateid) {
            uint32 handle;
            m_netModule->connectAsyn("127.0.0.1", 8005, &handle, this);
        }
        LOG_INFO("InnerNetCallback onDisconnect netid = %d", netid);
    }
    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) {
        if (result) {
            m_gateid = netid;
        } else {
            m_netModule->connectAsyn(host, port, &handle, this);
        }
        LOG_INFO("InnerNetCallback onConnect %s", (result ? "true" : "false"));
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

    LogModule *logModule = dynamic_cast<LogModule*>(getServer()->getModel(LOG_MODULE));
    // eg:
    g_money = logModule->create("monery", "yuanbao");
    g_money->setTarget(LOG_FILE);
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

    // eg:
    g_money->write(LV_INFO, "id:%d,type:%d,count:%d", 1001, 1, 50);
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