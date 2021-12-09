#include "sysutil.h"
#include "logmodule.h"

#include "luamodule.h"

LuaModule::LuaModule(const char *file) : m_inited(false), m_file(file) {
}

void LuaModule::luaInit() {
    L = luaL_newstate();
    luaL_openlibs(L);

    getGlobalNamespace(L)
        .beginClass<LuaModule>("LuaEngine")
            .addFunction("onInited", &LuaModule::onInited)
        .endClass();

    onLuaState(L);
}

void LuaModule::luaStart() {
    std::string path;
    SysUtil::getProcDir(path);
    path = path + "../script/" + m_file;
    if (luaL_dofile(L, path.c_str())) {
        LOG_INFO("LuaEngine::start script name [%s] error", m_file.c_str());
    }
}

void LuaModule::luaUpdate(uint32 now) {
    invoke("LuaUpdate", now);
}

void LuaModule::luaRecv(LuaRef ref) {
    invoke("LuaRecv", ref);
}

LuaRef LuaModule::luaNewTable() {
    return newTable(L);
}

void LuaModule::onInited() {
    m_inited = true;
    LOG_INFO("LuaEngine::onInited script true", m_file.c_str());
}