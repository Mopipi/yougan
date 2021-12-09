#ifndef _LOGIN_MODULE_H_
#define _LOGIN_MODULE_H_

#include "server.h"
#include "clockmodule.h"
#include "logmodule.h"
#include "netmodule.h"
#include "luamodule.h"

static const char LOGIN_MODULE[] = "LoginModule";

class LoginModule : public LuaModule {
    DISALLOW_COPY_AND_ASSIGN(LoginModule);
public:
    LoginModule();
    ~LoginModule();
public:
    virtual void onLuaState(lua_State *L);
public:
    virtual int init();
    virtual int start();
    virtual int update();
    virtual int stop();
    virtual int release();
public:
    void sendClient(uint32 plyaerId);
public:
    ClockModule *m_clockModule;
    NetModule *m_netModule;
};

#endif