#include <iostream>

#include "gatemodule.h"

int main(int argc, char *argv[]) {
    Server server("gate");
    server.addModel(LOG_MODULE, new LogModule);
    server.addModel(NET_MODULE, new NetModule);
    server.addModel(GATE_MODULE, new GateModule);

    server.run();
    return 0;
}