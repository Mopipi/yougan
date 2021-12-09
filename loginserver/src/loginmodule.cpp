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
    InnerNetCallback(NetModule* netModule, LoginModule *loginModule) : m_netModule(netModule), m_loginModule(loginModule){

    }
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("InnerNetCallback onAccept %d, %d", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {
        // eg:
        struct test {
            int a;
            int b;
        };
        test *t = (struct test*)data;
        LOG_INFO("InnerNetCallback::onRecv data { int a = %d; int b = %d}", t->a, t->b);
        LuaRef pkg = m_loginModule->luaNewTable();
        pkg["netid"] = netid;
        pkg["a"] = t->a;
        pkg["b"] = t->b;
        m_loginModule->luaRecv(pkg);
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
            struct {
                int b = 10;
                int c = 90;
            }a;
            m_netModule->send(m_gateid, &a, sizeof(a));
        } else {
            m_netModule->connectAsyn(host, port, &handle, this);
        }
        LOG_INFO("InnerNetCallback onConnect %s", (result ? "true" : "false"));
    }
private:
    NetID m_gateid;
    NetModule * m_netModule;
    LoginModule *m_loginModule;
};

// eg: timer
class TestTimer : public TimerCall {
public:
    TestTimer(ClockModule *clock) : m_clock(clock) {

    }
    virtual void onTimeout() {
        LOG_INFO("=======TestTimer:onTimeout=======");
        call();
    }
public:
    void call() {
        m_clock->timerAdd(new TestTimer(m_clock), 1000 * 5);
    }
private:
    ClockModule * m_clock;
};

LoginModule::LoginModule() : LuaModule("initLogin.lua") {

}

LoginModule::~LoginModule() {

}

void LoginModule::onLuaState(lua_State *L) {
    getGlobalNamespace(L)
        .deriveClass<LoginModule, LuaModule>("LoginModule")
        .addFunction("sendClient", &LoginModule::sendClient)
        .endClass();
}

int LoginModule::init() {
    EXPECT_ON_INIT(LOG_MODULE);

    LuaModule::luaInit();

    LogModule *logModule = dynamic_cast<LogModule*>(getServer()->getModel(LOG_MODULE));
    // eg:
    g_money = logModule->create("monery", "yuanbao", FILE_APPEND);
    g_money->setTarget(LOG_FILE);
    return Succeed;
}

int LoginModule::start() {
    EXPECT_ON_START(CLOCK_MODULE)
    DEPEND_ON_START(NET_MODULE);

    LuaModule::luaStart();

    m_netModule = dynamic_cast<NetModule*>(getServer()->getModel(NET_MODULE));

    ClientNetCallback *clientNetworkCallBack = new ClientNetCallback;
    m_netModule->listen("0.0.0.0", 8001, 1, clientNetworkCallBack);

    GameNetCallback *gameNetworkCallBack = new GameNetCallback;
    m_netModule->listen("0.0.0.0", 8002, 1, gameNetworkCallBack);

    InnerNetCallback *innerNetworkCallBack = new InnerNetCallback(m_netModule, this);
    m_netModule->listen("0.0.0.0", 8003, 1, innerNetworkCallBack);
    m_netModule->listen("0.0.0.0", 8004, 1, innerNetworkCallBack);

    //m_netModule->connect("127.0.0.1", 8004, innerNetworkCallBack);

    uint32 handle;
    m_netModule->connectAsyn("127.0.0.1", 8005, &handle, innerNetworkCallBack);

    // eg:
    g_money->write(LV_INFO, "id:%d,type:%d,count:%d", 1001, 1, 50);

    m_clockModule = dynamic_cast<ClockModule*>(getServer()->getModel(CLOCK_MODULE));
    TestTimer *timer = new TestTimer(m_clockModule);
    timer->call();
    return Succeed;
}

int LoginModule::update() {
    uint32 now = m_clockModule->getUtcTime();
    LuaModule::luaUpdate(now);
    return Pending;
}

int LoginModule::stop() {
    return Succeed;
}

int LoginModule::release() {
    return Succeed;
}

void LoginModule::sendClient(uint32 plyaerId) {
    LOG_INFO("LoginModule::sendClient plyaerId = %d", plyaerId);
}