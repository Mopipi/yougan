#ifndef _LUA_MODULE_H_
#define _LUA_MODULE_H_

#include "define.h"

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "module.h"

using namespace luabridge;

class LuaModule: public Module {
    DISALLOW_COPY_AND_ASSIGN(LuaModule);
public:
    LuaModule(const char *file);
    ~LuaModule() {}
public:
    virtual void onLuaState(lua_State *L) = 0;
public:
    void luaInit();
    void luaStart();
    void luaUpdate(uint32 now);
    void luaRecv(LuaRef ref);
    LuaRef luaNewTable();
public:
    void onInited();
public:
    template<typename P>
    void invoke(const char *funname, P p) {
        LuaRef func = getGlobal(L, funname);
        if (func.isFunction()) {
            func(p);
        }
    }
private:
    bool m_inited;
    std::string m_file;
    lua_State *L;
};
#endif