#ifndef _SERSER_H_
#define _SERSER_H_

#include "module.h"
#include "synclock.h"

static const char NET_MODULE[] = "NetModule";

class Server {
    DISALLOW_COPY_AND_ASSIGN(Server);

    typedef int (Module::*Callback)();

    typedef std::unordered_map<std::string, Module*> ModuleMap;
    typedef std::vector<Module*> ModuleQueue;
public:
    Server();
    virtual ~Server();
public:
    bool addModel(std::string name, Module *module);
    Module* getModel(std::string name);
    Semaphore* getMainLoopSem();
public:
    void run();
    void stop();
private:
    void loop(ModuleQueue& moduleQueue, Callback cb, ModuleQueue *updateQueue, bool ignore, int succeed, int pending);
private:
    bool m_exist;
    Semaphore m_loopSem;

    ModuleQueue m_moduleQueue;
    ModuleQueue m_initModuleQueue;
    ModuleQueue m_startModuleQueue;

    ModuleMap m_moduleMap;
};
#endif