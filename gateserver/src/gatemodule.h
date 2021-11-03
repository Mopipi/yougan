#ifndef _GATE_MODULE_H_
#define _GATE_MODULE_H_

#include "server.h"
#include "logmodule.h"
#include "netmodule.h"

static const char GATE_MODULE[] = "GateModule";

class GateModule : public Module {
    DISALLOW_COPY_AND_ASSIGN(GateModule);
public:
    GateModule();
    ~GateModule();
public:
    virtual int init();
    virtual int start();
    virtual int update();
    virtual int stop();
    virtual int release();
private:
    NetModule *m_netModule;
};
#endif