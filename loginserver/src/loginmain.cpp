#include <iostream>
#include "netmodule.h"

using namespace std;

class ClientNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("ClientNetCallback onAccept %d, %d\n", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {

    }

    virtual void onDisconnect(NetID netid) {

    }
    virtual void onConnect(bool result, int handle, NetID netid, Host host, Port port) {

    }

};

class GameNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("GameNetCallback onAccept %d, %d\n", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {

    }

    virtual void onDisconnect(NetID netid) {

    }
    virtual void onConnect(bool result, int handle, NetID netid, Host host, Port port) {

    }

};

class InnerNetCallback : public NetCallback {
public:
    virtual void onAccept(Port listenPort, NetID netid, Host host, Port port) {
        printf("InnerNetCallback onAccept %d, %d\n", host, port);
    }

    virtual void onRecv(NetID netid, const char *data, int length) {

    }

    virtual void onDisconnect(NetID netid) {

    }
    virtual void onConnect(bool result, int handle, NetID netid, Host host, Port port) {

    }

};

int main(int argc, char *argv[]) {

    NetModule netModule;
    netModule.init();
    netModule.start();

    ClientNetCallback *clientNetworkCallBack = new ClientNetCallback;
    netModule.listen(8001, 1, "0.0.0.0", clientNetworkCallBack);

    GameNetCallback *gameNetworkCallBack = new GameNetCallback;
    netModule.listen(8002, 1, "0.0.0.0", gameNetworkCallBack);

    InnerNetCallback *innerNetworkCallBack = new InnerNetCallback;
    netModule.listen(8003, 1, "0.0.0.0", innerNetworkCallBack);
    netModule.listen(8004, 1, "0.0.0.0", innerNetworkCallBack);

    while (true)
    {
        netModule.update();
        Sleep(100);
    }
    return 0;
}