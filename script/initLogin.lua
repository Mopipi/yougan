print("LuaInit script", LuaEngine)

function LuaInit(server)
    server:onInited()
    server:sendClient(9999)
end

local lastTime = 0
function LuaUpdate(time)
    if time >= lastTime then
        lastTime = time
        print("LuaUpdate:" ..os.date("%Y-%m-%d %H:%M:%S", time))
        lastTime = lastTime + 5
    end
end

function LuaRecv(pkg)
    print("LuaRecv netId = ", pkg.netid, pkg.a, pkg.b)
end