#include <iostream>

#include "loginmodule.h"

int main(int argc, char *argv[]) {
    Server server("login");
    server.addModel(CLOCK_MODULE, new ClockModule);
    server.addModel(LOG_MODULE, new LogModule);
    server.addModel(NET_MODULE, new NetModule);
    server.addModel(LOGIN_MODULE, new LoginModule);
  
    server.run();
    return 0;
}