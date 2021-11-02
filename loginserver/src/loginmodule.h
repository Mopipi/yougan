#ifndef _LOGIN_MODULE_H_
#define _LOGIN_MODULE_H_

#include "server.h"
#include "logmodule.h"
#include "netmodule.h"

static const char LOGIN_MODULE[] = "LoginModule";

class LoginModule : public Module {
    DISALLOW_COPY_AND_ASSIGN(LoginModule);
public:
    LoginModule();
    ~LoginModule();
public:
    virtual int init();
    virtual int start();
    virtual int update();
    virtual int stop();
    virtual int release();
public:
    NetModule *m_netModule;
};

#endif