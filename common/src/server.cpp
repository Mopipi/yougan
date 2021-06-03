#include "server.h"

Server::Server():m_exist(false) {

}

Server::~Server() {

}

bool Server::addModel(std::string name, Module *module) {
    if (module == 0)
        return false;

    if (m_moduleMap.find(name) != m_moduleMap.end())
        return false;

    m_moduleMap.insert(std::make_pair(name, module));
    module->m_server = this;
    m_moduleQueue.push_back(module);
    return true;
}

Module* Server::getModel(std::string name) {
    ModuleMap::iterator iter = m_moduleMap.find(name);
    return iter != m_moduleMap.end() ? iter->second : 0;
}

Semaphore* Server::getMainLoopSem() {
    return &m_loopSem;
}

void Server::run() {
    loop(m_moduleQueue, &Module::init, &m_initModuleQueue, false, Module::ST_Inited, Module::ST_Initing);
    loop(m_initModuleQueue, &Module::start, &m_startModuleQueue, false, Module::ST_Started, Module::ST_Starting);
    loop(m_startModuleQueue, &Module::update, 0, false, Module::ST_Updated, Module::ST_Updating);

    std::reverse(m_startModuleQueue.begin(), m_startModuleQueue.end());
    loop(m_startModuleQueue, &Module::stop, 0, true, Module::ST_Stopped, Module::ST_Stopping);

    std::reverse(m_startModuleQueue.begin(), m_startModuleQueue.end());
    loop(m_startModuleQueue, &Module::release, 0, true, Module::ST_Released, Module::ST_Releasing);

}

void Server::stop() {
    m_exist = true;
}

void Server::loop(ModuleQueue& moduleQueue, Callback cb, ModuleQueue *updateQueue, bool ignore, int succeed, int pending) {
    ModuleQueue loopQueue = moduleQueue;
    while (!loopQueue.empty() && (ignore || !m_exist)) {
        bool wait = false;
        ModuleQueue::iterator iter = loopQueue.begin();
        while (iter != loopQueue.end()) {
            Module* module = *iter;
            module->m_state = pending;
            switch ((module->*cb)()) {
            case Module::Succeed:
                if (updateQueue != 0)
                    updateQueue->push_back(module);

                module->m_state = succeed;
                iter = loopQueue.erase(iter);
                break;
            case Module::Pending:
                ++iter;
                wait = true;
                break;
            case Module::Failed:
                module->m_state = Module::ST_Failed;
                iter = loopQueue.erase(iter);
                break;
            }
        }
        if (wait) {
            m_loopSem.wait(10);
        }
    }

}